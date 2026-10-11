#include <gtest/gtest.h>
#include "feedback/AIFeedback.h"
#include "feedback/FeedbackGenerator.h"
#include "ai/MockLLMClient.h"
#include "ai/NullLLMClient.h"
#include "ai/AiExceptions.h"
#include "interview/InterviewSession.h"
#include "core/MCQQuestion.h"
#include "core/Difficulty.h"
#include <memory>

// Helper to build a tiny finished session
static std::unique_ptr<InterviewSession> makeFinishedSession(const std::string& userId) {
    auto session = std::make_unique<InterviewSession>(userId, "CSE", "Data Structures");
    auto q = std::make_shared<MCQQuestion>("q1", "DS", "Arrays", Difficulty::EASY,
        "What is an array?", "Contiguous memory", std::vector<std::string>{"Linked list", "Contiguous memory"}, "B");
    session->start();
    session->submitAnswer(q, "B", 10);
    session->finish();
    return session;
}

TEST(AIFeedbackTest, SuccessPathReturnsAiGeneratedReport) {
    std::string goodResponse = R"({
        "overall_summary": "Great performance",
        "strengths": ["Quick answers"],
        "weaknesses": [],
        "communication_clarity": 5,
        "answer_structure": 4,
        "confidence_cues": 4,
        "suggestions": ["Keep it up"]
    })";
    auto mock = std::make_shared<MockLLMClient>(goodResponse);
    AIFeedback feedbackGen(mock);

    auto session = makeFinishedSession("student1");
    auto report = feedbackGen.generateFeedback(*session);

    EXPECT_TRUE(report.isAiGenerated);
    EXPECT_EQ(report.overallSummary, "Great performance");
    EXPECT_EQ(report.communicationClarity, 5);
    EXPECT_FALSE(report.notice.empty()); // should have "AI-generated" notice
}

TEST(AIFeedbackTest, ParseFailureFallsBackToRuleBasedFeedback) {
    auto mock = std::make_shared<MockLLMClient>("Not JSON at all -- malformed {{");
    AIFeedback feedbackGen(mock);

    auto session = makeFinishedSession("student2");
    auto report = feedbackGen.generateFeedback(*session);

    EXPECT_FALSE(report.isAiGenerated);
    // Should still have a valid percentage
    EXPECT_GE(report.percentage, 0.0);
    // Notice should explain fallback
    EXPECT_FALSE(report.notice.empty());
}

TEST(AIFeedbackTest, NullClientFallsBackToRuleBased) {
    auto nullClient = std::make_shared<NullLLMClient>();
    AIFeedback feedbackGen(nullClient);

    auto session = makeFinishedSession("student3");
    auto report = feedbackGen.generateFeedback(*session);

    EXPECT_FALSE(report.isAiGenerated);
    EXPECT_GE(report.percentage, 0.0);
}

TEST(AIFeedbackTest, NetworkErrorFallsBackToRuleBased) {
    auto mock = std::make_shared<MockLLMClient>();
    LLMResponse errorResp;
    errorResp.success = false;
    errorResp.errorMessage = "Network unreachable";
    mock->enqueueResponse(errorResp);
    AIFeedback feedbackGen(mock);

    auto session = makeFinishedSession("student4");
    auto report = feedbackGen.generateFeedback(*session);

    EXPECT_FALSE(report.isAiGenerated);
}

TEST(AIFeedbackTest, IsConfiguredFalseWithNullClient) {
    auto nullClient = std::make_shared<NullLLMClient>();
    AIFeedback feedbackGen(nullClient);
    EXPECT_FALSE(feedbackGen.isConfigured());
}

TEST(AIFeedbackTest, IsConfiguredTrueWithRealClient) {
    auto mock = std::make_shared<MockLLMClient>("{}");
    AIFeedback feedbackGen(mock);
    EXPECT_TRUE(feedbackGen.isConfigured());
}
