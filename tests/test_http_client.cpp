#include <gtest/gtest.h>
#include "ai/HttpClient.h"
#include "ai/AiExceptions.h"
#include "ai/IHttpTransport.h"
#include <vector>
#include <string>
#include <stdexcept>

// Fake transport for injecting controlled HTTP responses
class FakeTransport : public IHttpTransport {
public:
    struct ResponseSpec {
        int statusCode{200};
        std::string body{"ok"};
        bool throwTimeout{false};
        bool throwNetwork{false};
    };

    std::vector<ResponseSpec> responses;
    int callCount{0};

    HttpResponse executePost(const std::string& /*url*/,
                             const std::vector<std::string>& /*headers*/,
                             const std::string& /*jsonBody*/,
                             int /*timeout*/) override {
        int idx = std::min(callCount, static_cast<int>(responses.size()) - 1);
        callCount++;
        const auto& spec = responses[static_cast<size_t>(idx)];
        if (spec.throwTimeout) throw TimeoutException("Fake timeout");
        if (spec.throwNetwork) throw NetworkException("Fake network error");
        return HttpResponse{spec.statusCode, spec.body};
    }
};

TEST(HttpClientTest, SuccessOnFirstAttempt) {
    auto transport = std::make_shared<FakeTransport>();
    transport->responses.push_back({200, R"({"ok":true})"});
    HttpClient client(transport);
    client.setEnableSleepInBackoff(false);
    auto resp = client.post("http://fake", {}, "{}");
    EXPECT_EQ(resp.statusCode, 200);
    EXPECT_EQ(transport->callCount, 1);
}

TEST(HttpClientTest, RetriesOn429ThenSucceeds) {
    auto transport = std::make_shared<FakeTransport>();
    transport->responses.push_back({429, "rate limited"});
    transport->responses.push_back({429, "rate limited"});
    transport->responses.push_back({200, R"({"ok":true})"});
    HttpClient client(transport);
    client.setEnableSleepInBackoff(false);
    auto resp = client.post("http://fake", {}, "{}");
    EXPECT_EQ(resp.statusCode, 200);
    EXPECT_EQ(transport->callCount, 3);
}

TEST(HttpClientTest, RetriesOn500ThenSucceeds) {
    auto transport = std::make_shared<FakeTransport>();
    transport->responses.push_back({500, "server error"});
    transport->responses.push_back({200, R"({"ok":true})"});
    HttpClient client(transport);
    client.setEnableSleepInBackoff(false);
    auto resp = client.post("http://fake", {}, "{}");
    EXPECT_EQ(resp.statusCode, 200);
    EXPECT_EQ(transport->callCount, 2);
}

TEST(HttpClientTest, NoRetryOn4xx) {
    auto transport = std::make_shared<FakeTransport>();
    transport->responses.push_back({401, "unauthorized"});
    HttpClient client(transport);
    client.setEnableSleepInBackoff(false);
    EXPECT_THROW(client.post("http://fake", {}, "{}"), ApiException);
    EXPECT_EQ(transport->callCount, 1); // Only 1 attempt, no retry
}

TEST(HttpClientTest, RetriesOnTimeoutUpToMaxThenThrows) {
    auto transport = std::make_shared<FakeTransport>();
    transport->responses.push_back({0, "", true, false}); // timeout
    transport->responses.push_back({0, "", true, false}); // timeout
    transport->responses.push_back({0, "", true, false}); // timeout
    HttpClient client(transport);
    client.setEnableSleepInBackoff(false);
    client.setMaxRetries(3);
    EXPECT_THROW(client.post("http://fake", {}, "{}"), TimeoutException);
    EXPECT_EQ(transport->callCount, 3);
}

TEST(HttpClientTest, MaskApiKey) {
    EXPECT_EQ(HttpClient::maskApiKey("sk-proj-abc1234"), "sk-****1234");
    EXPECT_EQ(HttpClient::maskApiKey("AB"), "****");
    EXPECT_EQ(HttpClient::maskApiKey(""), "[EMPTY]");
}

TEST(HttpClientTest, ExhaustsRetriesOn429Throws) {
    auto transport = std::make_shared<FakeTransport>();
    transport->responses.push_back({429, "rate limit"});
    transport->responses.push_back({429, "rate limit"});
    transport->responses.push_back({429, "rate limit"});
    HttpClient client(transport);
    client.setEnableSleepInBackoff(false);
    client.setMaxRetries(3);
    EXPECT_THROW(client.post("http://fake", {}, "{}"), RateLimitException);
    EXPECT_EQ(transport->callCount, 3);
}
