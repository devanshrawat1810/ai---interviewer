#pragma once

#include "ai/ILLMClient.h"
#include "ai/HttpClient.h"
#include <memory>
#include <string>

// OOP Concept: Concrete Strategy (Polymorphism)
// Implements ILLMClient for the Anthropic Messages API.
// Documentation Reference: Anthropic Messages API Reference v1 (Doc version 2023-06-01).
class AnthropicClient : public ILLMClient {
private:
    std::string apiKey_;
    std::string model_;
    std::string baseUrl_;
    int timeoutSeconds_{30};
    std::shared_ptr<HttpClient> httpClient_;

public:
    AnthropicClient(std::string apiKey,
                    std::string model = "claude-3-5-sonnet-20241022",
                    std::string baseUrl = "https://api.anthropic.com/v1/messages",
                    int timeoutSeconds = 30,
                    std::shared_ptr<HttpClient> httpClient = nullptr);

    ~AnthropicClient() override = default;

    LLMResponse complete(const LLMRequest& request) override;
    std::string getProviderName() const override;
};
