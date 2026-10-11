#pragma once

#include "ai/ILLMClient.h"
#include "ai/AiExceptions.h"

// OOP Concept: Null Object Pattern
// Avoids null-pointer checks by providing a safe fallback that always reports unavailability.
class NullLLMClient : public ILLMClient {
public:
    ~NullLLMClient() override = default;

    LLMResponse complete(const LLMRequest& /*request*/) override {
        throw AiUnavailableException("AI service is disabled: No API key or provider configured.");
    }

    std::string getProviderName() const override {
        return "NullLLMClient (Disabled)";
    }
};
