#pragma once

#include "ai/ILLMClient.h"
#include <memory>
#include <string>

// OOP Concept: Factory Pattern
// Centralizes instantiation of LLM client strategies from environment configuration.
class LLMClientFactory {
public:
    static std::shared_ptr<ILLMClient> createClientFromEnv();

    static std::shared_ptr<ILLMClient> createClient(const std::string& provider,
                                                   const std::string& apiKey,
                                                   const std::string& model = "",
                                                   const std::string& baseUrl = "",
                                                   int timeoutSeconds = 30);
};
