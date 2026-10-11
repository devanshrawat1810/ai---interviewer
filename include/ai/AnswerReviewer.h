#pragma once

#include "ai/ILLMClient.h"
#include "ai/PromptLibrary.h"
#include "ai/UsageTracker.h"
#include <memory>
#include <string>
#include <vector>

struct AnswerReviewResult {
    int rating{3};
    std::string feedback;
    std::string improvedAnswer;
    std::vector<std::string> strengths;
    std::vector<std::string> improvements;
    bool success{false};
    std::string errorMessage;
};

// OOP Concept: Single Responsibility & Rate Limiting Guard
// Provides on-demand coaching on individual responses with daily quota enforcement (max 5/day).
class AnswerReviewer {
private:
    std::shared_ptr<ILLMClient> llmClient_;
    std::shared_ptr<PromptLibrary> promptLibrary_;
    std::shared_ptr<UsageTracker> usageTracker_;
    int maxReviewsPerDay_{5};

public:
    AnswerReviewer(std::shared_ptr<ILLMClient> llmClient = nullptr,
                   std::shared_ptr<PromptLibrary> promptLib = nullptr,
                   std::shared_ptr<UsageTracker> tracker = nullptr,
                   int maxReviewsPerDay = 5);

    AnswerReviewResult reviewAnswer(const std::string& userId,
                                   const std::string& questionText,
                                   const std::string& studentAnswer);

    bool isAvailable() const noexcept;
};
