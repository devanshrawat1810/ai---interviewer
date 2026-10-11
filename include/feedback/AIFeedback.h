#pragma once

#include "feedback/FeedbackGenerator.h"

// OOP Concept: Polymorphism & Extensibility
// Extension point for remote LLM-powered feedback evaluation.
// Reads AI_API_KEY from environment; gracefully falls back to descriptive status
// if the key is not present so the application never breaks.
class AIFeedback : public FeedbackGenerator {
private:
    std::string apiKey_;

public:
    AIFeedback();
    explicit AIFeedback(std::string apiKey);
    ~AIFeedback() override = default;

    FeedbackReport generateFeedback(const InterviewSession& session) override;

    bool isConfigured() const noexcept;
    const std::string& getApiKey() const noexcept;
};
