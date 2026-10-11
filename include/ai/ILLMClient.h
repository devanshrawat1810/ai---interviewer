#pragma once

#include <string>

struct LLMRequest {
    std::string systemPrompt;
    std::string userMessage;
    int maxTokens{1024};
    double temperature{0.7};
};

struct LLMResponse {
    std::string content;
    int promptTokens{0};
    int completionTokens{0};
    bool success{true};
    std::string errorMessage;
};

// OOP Concept: Strategy Pattern & Abstraction
// Pure virtual interface abstracting generative AI providers behind a unified contract.
class ILLMClient {
public:
    virtual ~ILLMClient() = default;

    virtual LLMResponse complete(const LLMRequest& request) = 0;
    virtual std::string getProviderName() const = 0;
};
