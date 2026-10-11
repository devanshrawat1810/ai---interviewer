#include <gtest/gtest.h>
#include "ai/ResponseParser.h"
#include "ai/AiExceptions.h"

// ResponseParser static helpers

TEST(ResponseParserTest, ParsesPlainJson) {
    std::string raw = R"({"score": 5, "feedback": "Great!"})";
    auto j = ResponseParser::parseJson(raw);
    EXPECT_EQ(j.at("score").get<int>(), 5);
    EXPECT_EQ(j.at("feedback").get<std::string>(), "Great!");
}

TEST(ResponseParserTest, StripsMarkdownCodeFence) {
    std::string raw = "```json\n{\"score\": 3}\n```";
    auto j = ResponseParser::parseJson(raw);
    EXPECT_EQ(j.at("score").get<int>(), 3);
}

TEST(ResponseParserTest, StripsBackticksFenceWithoutLanguage) {
    std::string raw = "```\n{\"key\": \"value\"}\n```";
    auto j = ResponseParser::parseJson(raw);
    EXPECT_EQ(j.at("key").get<std::string>(), "value");
}

TEST(ResponseParserTest, RejectsInvalidJsonThrows) {
    std::string raw = "This is not JSON at all.";
    EXPECT_THROW(ResponseParser::parseJson(raw), ParseException);
}

TEST(ResponseParserTest, ExtractsFeedbackReport) {
    std::string raw = R"({
        "overall_summary": "Good effort",
        "strengths": ["Clear answers"],
        "weaknesses": ["Needs depth"],
        "communication_clarity": 4,
        "answer_structure": 3,
        "confidence_cues": 4,
        "topic_notes": {"OOP": "Solid"},
        "suggestions": ["Practice daily"]
    })";
    auto j = ResponseParser::parseJson(raw);
    auto report = ResponseParser::extractFeedbackReport(j);
    EXPECT_EQ(report.overallSummary, "Good effort");
    EXPECT_EQ(report.strengths.size(), 1u);
    EXPECT_EQ(report.strengths[0], "Clear answers");
    EXPECT_EQ(report.weaknesses.size(), 1u);
    EXPECT_EQ(report.communicationClarity, 4);
    EXPECT_EQ(report.answerStructure, 3);
    EXPECT_EQ(report.confidenceCues, 4);
    EXPECT_EQ(report.suggestions.size(), 1u);
}

TEST(ResponseParserTest, ClampsClarityScoreTo1to5) {
    std::string raw = R"({
        "overall_summary": "test",
        "communication_clarity": 9,
        "answer_structure": 0,
        "confidence_cues": -1
    })";
    auto j = ResponseParser::parseJson(raw);
    auto report = ResponseParser::extractFeedbackReport(j);
    EXPECT_EQ(report.communicationClarity, 5);
    EXPECT_EQ(report.answerStructure, 1);
    EXPECT_EQ(report.confidenceCues, 1);
}

TEST(ResponseParserTest, MissingOptionalFieldsDefaultGracefully) {
    std::string raw = R"({"overall_summary": "minimal"})";
    auto j = ResponseParser::parseJson(raw);
    auto report = ResponseParser::extractFeedbackReport(j);
    EXPECT_TRUE(report.strengths.empty());
    EXPECT_TRUE(report.weaknesses.empty());
    EXPECT_TRUE(report.suggestions.empty());
    EXPECT_EQ(report.communicationClarity, 3); // default mid value
}

TEST(ResponseParserTest, ParsesAnswerReviewResult) {
    std::string raw = R"({
        "rating": 4,
        "feedback": "Good answer",
        "strengths": ["Concise"],
        "improvements": ["Add example"],
        "improved_answer": "A better answer with example..."
    })";
    auto j = ResponseParser::parseJson(raw);
    auto result = ResponseParser::extractAnswerReview(j);
    EXPECT_EQ(result.rating, 4);
    EXPECT_EQ(result.feedback, "Good answer");
    EXPECT_EQ(result.strengths.size(), 1u);
    EXPECT_EQ(result.improvements.size(), 1u);
    EXPECT_FALSE(result.improvedAnswer.empty());
}
