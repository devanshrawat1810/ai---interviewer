#pragma once

#include "feedback/FeedbackGenerator.h"
#include "feedback/RuleBasedFeedback.h"
#include "ai/ILLMClient.h"
#include "ai/PromptLibrary.h"
#include <memory>
#include <string>

// OOP Concept: Polymorphism & Decorator/Fallback Pattern
// Implements FeedbackGenerator using an LLM client with seamless offline fallback to RuleBasedFeedback.
class AIFeedback : public FeedbackGenerator {
private:
    std::shared_ptr<ILLMClient> llmClient_;
    std::shared_ptr<PromptLibrary> promptLibrary_;
    RuleBasedFeedback fallbackGenerator_;

public:
    AIFeedback();
    explicit AIFeedback(std::shared_ptr<ILLMClient> llmClient,
                        std::shared_ptr<PromptLibrary> promptLib = nullptr);
    ~AIFeedback() override = default;

    FeedbackReport generateFeedback(const InterviewSession& session) override;

    bool isConfigured() const noexcept;
    std::string getProviderName() const;
};
