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
};

// OOP Concept: Abstraction & Open/Closed Principle
// Defines the contract for generating analytical feedback reports from interview sessions.
class FeedbackGenerator {
public:
    virtual ~FeedbackGenerator() = default;
    virtual FeedbackReport generateFeedback(const InterviewSession& session) = 0;
};
