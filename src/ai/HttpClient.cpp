#include "ai/HttpClient.h"
#include <chrono>
#include <thread>
#include <iostream>

#if defined(AI_INTERVIEWER_HAS_CURL)
#include <curl/curl.h>

namespace {
    // RAII global curl initializer
    class CurlGlobalGuard {
    public:
        CurlGlobalGuard() {
            curl_global_init(CURL_GLOBAL_ALL);
        }
        ~CurlGlobalGuard() {
            curl_global_cleanup();
        }
    };

    static size_t curlWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t totalBytes = size * nmemb;
        auto* str = static_cast<std::string*>(userp);
        str->append(static_cast<char*>(contents), totalBytes);
        return totalBytes;
    }

    class DefaultCurlTransport : public IHttpTransport {
    public:
        HttpResponse executePost(const std::string& url,
                                 const std::vector<std::string>& headers,
                                 const std::string& jsonBody,
                                 int timeoutSeconds) override {
            static CurlGlobalGuard guard;
            CURL* curl = curl_easy_init();
            if (!curl) {
                throw NetworkException("Failed to initialize curl handle");
            }

            std::string responseBuffer;
            struct curl_slist* headerList = nullptr;
            for (const auto& h : headers) {
                headerList = curl_slist_append(headerList, h.c_str());
            }

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

            CURLcode res = curl_easy_perform(curl);
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

            curl_slist_free_all(headerList);
            curl_easy_cleanup(curl);

            if (res == CURLE_OPERATION_TIMEDOUT) {
                throw TimeoutException("Request timed out after " + std::to_string(timeoutSeconds) + "s");
            }
            if (res != CURLE_OK) {
                throw NetworkException(curl_easy_strerror(res));
            }

            return HttpResponse{static_cast<int>(httpCode), responseBuffer};
        }
    };
}
#else
namespace {
    class DefaultCurlTransport : public IHttpTransport {
    public:
        HttpResponse executePost(const std::string& /*url*/,
                                 const std::vector<std::string>& /*headers*/,
                                 const std::string& /*jsonBody*/,
                                 int /*timeoutSeconds*/) override {
            throw NetworkException("libcurl is not compiled into this binary build");
        }
    };
}
#endif

HttpClient::HttpClient(std::shared_ptr<IHttpTransport> transport)
    : transport_(transport ? std::move(transport) : std::make_shared<DefaultCurlTransport>()) {}

HttpResponse HttpClient::post(const std::string& url,
                              const std::vector<std::string>& headers,
                              const std::string& jsonBody,
                              int timeoutSeconds) {
    int attempts = 0;
    std::string lastErrorMessage;

    while (attempts < maxRetries_) {
        attempts++;
        try {
            HttpResponse resp = transport_->executePost(url, headers, jsonBody, timeoutSeconds);

            // Success (2xx, 3xx)
            if (resp.statusCode >= 200 && resp.statusCode < 400) {
                return resp;
            }

            // Rate limit (429): retry with backoff
            if (resp.statusCode == 429) {
                lastErrorMessage = "HTTP 429 Rate limit exceeded: " + resp.body;
                if (attempts >= maxRetries_) {
                    throw RateLimitException(resp.body);
                }
            }
            // Server errors (5xx): retry with backoff
            else if (resp.statusCode >= 500) {
                lastErrorMessage = "HTTP " + std::to_string(resp.statusCode) + " Server Error: " + resp.body;
                if (attempts >= maxRetries_) {
                    throw ApiException(resp.statusCode, resp.body);
                }
            }
            // Client errors (4xx except 429): do NOT retry
            else if (resp.statusCode >= 400 && resp.statusCode < 500) {
                throw ApiException(resp.statusCode, resp.body);
            } else {
                return resp;
            }
        } catch (const TimeoutException& te) {
            lastErrorMessage = te.what();
            if (attempts >= maxRetries_) {
                throw;
            }
        } catch (const NetworkException& ne) {
            lastErrorMessage = ne.what();
            if (attempts >= maxRetries_) {
                throw;
            }
        }

        // Exponential backoff delay
        if (enableSleepInBackoff_) {
            int backoffDelay = baseBackoffMs_ * (1 << (attempts - 1));
            std::this_thread::sleep_for(std::chrono::milliseconds(backoffDelay));
        }
    }

    throw NetworkException("Failed after " + std::to_string(maxRetries_) + " attempts. Last error: " + lastErrorMessage);
}

std::string HttpClient::maskApiKey(const std::string& apiKey) {
    if (apiKey.empty()) {
        return "[EMPTY]";
    }
    if (apiKey.size() <= 8) {
        return "****";
    }
    return apiKey.substr(0, 3) + "****" + apiKey.substr(apiKey.size() - 4);
}
