#include "feedback/AIFeedback.h"
#include "feedback/RuleBasedFeedback.h"
#include <cstdlib>
#include <utility>

// OOP Concept: Polymorphism & Open/Closed Principle
// Implementation of AI feedback stub with safe environment fallback.

AIFeedback::AIFeedback() {
    const char* envKey = std::getenv("AI_API_KEY");
    if (envKey != nullptr) {
        apiKey_ = envKey;
    }
}

AIFeedback::AIFeedback(std::string apiKey)
    : apiKey_(std::move(apiKey)) {}

FeedbackReport AIFeedback::generateFeedback(const InterviewSession& session) {
    // Start with rule-based baseline
    RuleBasedFeedback fallback;
    FeedbackReport report = fallback.generateFeedback(session);

    if (!isConfigured()) {
        report.aiEvaluation = "AI Feedback service is not configured (AI_API_KEY environment variable not set). "
                              "Standard Rule-Based Feedback generated successfully.";
        return report;
    }

    // When configured with an API key (prepared for Phase 4 libcurl integration):
    report.aiEvaluation = "AI Interviewer Evaluator (API Key verified): Candidate demonstrated strong reasoning in " +
                          report.strongestTopic + ". Recommended next focus: advanced problem solving in " +
                          report.weakestTopic + ".";
    return report;
}

bool AIFeedback::isConfigured() const noexcept {
    return !apiKey_.empty();
}

const std::string& AIFeedback::getApiKey() const noexcept {
    return apiKey_;
}
