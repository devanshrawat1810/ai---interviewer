#pragma once

#include "ai/ILLMClient.h"
#include "ai/HttpClient.h"
#include <memory>
#include <string>

// OOP Concept: Concrete Strategy (Polymorphism)
// Implements ILLMClient for OpenAI-compatible endpoints (OpenAI, Gemini OpenAI endpoint, Groq, Ollama).
// Documentation Reference: OpenAI Chat Completions API Specification v1 (2024).
class OpenAICompatibleClient : public ILLMClient {
private:
    std::string apiKey_;
    std::string model_;
    std::string baseUrl_;
    int timeoutSeconds_{30};
    std::shared_ptr<HttpClient> httpClient_;

public:
    OpenAICompatibleClient(std::string apiKey,
                           std::string model = "gpt-4o-mini",
                           std::string baseUrl = "https://api.openai.com/v1/chat/completions",
                           int timeoutSeconds = 30,
                           std::shared_ptr<HttpClient> httpClient = nullptr);

    ~OpenAICompatibleClient() override = default;

    LLMResponse complete(const LLMRequest& request) override;
    std::string getProviderName() const override;
};
