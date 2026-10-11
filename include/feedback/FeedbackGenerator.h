#pragma once

#include <string>
#include <vector>
#include <map>
#include "core/Difficulty.h"

class InterviewSession;

// Structured evaluation report generated at the end of an interview
struct FeedbackReport {
    std::string sessionId;
    std::string userId;
    std::string subjectId;
    int totalScore{0};
    int maxScore{0};
    double percentage{0.0};
    std::map<Difficulty, int> scoreByDifficulty;
    std::map<Difficulty, int> maxByDifficulty;
    std::string strongestTopic;
    std::string weakestTopic;
    std::vector<std::string> suggestions;
    std::string aiEvaluation; // Optional commentary from AI generator

    // Phase 4: Extended AI Qualitative Metrics
    bool isAiGenerated{false};
    std::string overallSummary;
    std::vector<std::string> strengths;
    std::vector<std::string> weaknesses;
    std::map<std::string, std::string> perTopicNotes;
    int communicationClarity{3}; // Scale 1 - 5
    int answerStructure{3};      // Scale 1 - 5
    int confidenceCues{3};       // Scale 1 - 5
    std::string notice;          // Fallback notice or provider metadata
};

// OOP Concept: Abstraction & Open/Closed Principle
// Defines the contract for generating analytical feedback reports from interview sessions.
class FeedbackGenerator {
public:
    virtual ~FeedbackGenerator() = default;
    virtual FeedbackReport generateFeedback(const InterviewSession& session) = 0;
};
