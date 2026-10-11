#include <gtest/gtest.h>
#include "ai/LLMClientFactory.h"
#include "ai/NullLLMClient.h"
#include "ai/MockLLMClient.h"
#include "ai/AnthropicClient.h"
#include "ai/OpenAICompatibleClient.h"
#include "ai/AiExceptions.h"

TEST(LLMClientFactoryTest, MissingKeyReturnsNullClient) {
    auto client = LLMClientFactory::createClient("openai", "");
    ASSERT_NE(client, nullptr);
    EXPECT_EQ(client->getProviderName(), "NullLLMClient (Disabled)");
    // NullLLMClient should throw AiUnavailableException on use
    LLMRequest req;
    req.userMessage = "hello";
    EXPECT_THROW(client->complete(req), AiUnavailableException);
}

TEST(LLMClientFactoryTest, OpenAIProviderCreatesOpenAICompatibleClient) {
    auto client = LLMClientFactory::createClient("openai", "fake_key_123");
    ASSERT_NE(client, nullptr);
    EXPECT_NE(client->getProviderName().find("OpenAI-Compatible"), std::string::npos);
}

TEST(LLMClientFactoryTest, AnthropicProviderCreatesAnthropicClient) {
    auto client = LLMClientFactory::createClient("anthropic", "fake_key_123");
    ASSERT_NE(client, nullptr);
    EXPECT_NE(client->getProviderName().find("Anthropic"), std::string::npos);
}

TEST(LLMClientFactoryTest, ClaudeAliasCreatesAnthropicClient) {
    auto client = LLMClientFactory::createClient("claude", "fake_key_123");
    ASSERT_NE(client, nullptr);
    EXPECT_NE(client->getProviderName().find("Anthropic"), std::string::npos);
}

TEST(LLMClientFactoryTest, GeminiProviderCreatesOpenAICompatibleClient) {
    auto client = LLMClientFactory::createClient("gemini", "fake_key_123");
    ASSERT_NE(client, nullptr);
    EXPECT_NE(client->getProviderName().find("OpenAI-Compatible"), std::string::npos);
}

TEST(LLMClientFactoryTest, MockProviderCreatesMockClient) {
    auto client = LLMClientFactory::createClient("mock", "");
    ASSERT_NE(client, nullptr);
    EXPECT_EQ(client->getProviderName(), "MockLLMClient (Test)");
}

TEST(MockLLMClientTest, ReturnsDefaultResponse) {
    MockLLMClient mock(R"({"answer": 42})");
    LLMRequest req;
    req.userMessage = "what is 6x7?";
    auto resp = mock.complete(req);
    EXPECT_TRUE(resp.success);
    EXPECT_EQ(resp.content, R"({"answer": 42})");
}

TEST(MockLLMClientTest, ReturnsQueuedScriptedResponses) {
    MockLLMClient mock;
    LLMResponse r1; r1.success = true; r1.content = "first";
    LLMResponse r2; r2.success = true; r2.content = "second";
    mock.enqueueResponse(r1);
    mock.enqueueResponse(r2);
    LLMRequest req;
    EXPECT_EQ(mock.complete(req).content, "first");
    EXPECT_EQ(mock.complete(req).content, "second");
}

TEST(MockLLMClientTest, SimulatesFailure) {
    MockLLMClient mock;
    mock.setSimulateError(true, "API down");
    LLMRequest req;
    auto resp = mock.complete(req);
    EXPECT_FALSE(resp.success);
    EXPECT_EQ(resp.errorMessage, "API down");
}

TEST(MockLLMClientTest, RecordsRequests) {
    MockLLMClient mock;
    LLMRequest r1; r1.userMessage = "question one";
    LLMRequest r2; r2.userMessage = "question two";
    mock.complete(r1);
    mock.complete(r2);
    ASSERT_EQ(mock.getRecordedRequests().size(), 2u);
    EXPECT_EQ(mock.getRecordedRequests()[0].userMessage, "question one");
    EXPECT_EQ(mock.getRecordedRequests()[1].userMessage, "question two");
}
