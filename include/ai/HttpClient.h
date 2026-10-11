#pragma once

#include "ai/AiExceptions.h"
#include "ai/IHttpTransport.h"
#include <memory>
#include <string>
#include <vector>

// OOP Concept: Encapsulation & RAII Pattern
// Encapsulates network operations, lifecycle cleanup, backoff retry policies, and credential security.
class HttpClient {
private:
    std::shared_ptr<IHttpTransport> transport_;
    int maxRetries_{3};
    int baseBackoffMs_{100};
    bool enableSleepInBackoff_{true};

public:
    explicit HttpClient(std::shared_ptr<IHttpTransport> transport = nullptr);
    virtual ~HttpClient() = default;

    // Execute HTTP POST with automated retry on timeouts, 429, and 5xx
    HttpResponse post(const std::string& url,
                      const std::vector<std::string>& headers,
                      const std::string& jsonBody,
                      int timeoutSeconds = 30);

    void setMaxRetries(int retries) noexcept { maxRetries_ = retries; }
    void setBaseBackoffMs(int ms) noexcept { baseBackoffMs_ = ms; }
    void setEnableSleepInBackoff(bool enable) noexcept { enableSleepInBackoff_ = enable; }

    // Mask API key for logs/debug (e.g. sk-****1234)
    static std::string maskApiKey(const std::string& apiKey);
};
