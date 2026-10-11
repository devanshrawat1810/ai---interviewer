#include "feedback/RuleBasedFeedback.h"
#include "interview/InterviewSession.h"
#include <map>

// OOP Concept: Polymorphism & Algorithmic Diagnosis
// Implements deterministic rule-based analysis on completed sessions.

FeedbackReport RuleBasedFeedback::generateFeedback(const InterviewSession& session) {
    FeedbackReport report;
    report.sessionId = session.getId();
    report.userId = session.getUserId();
    report.subjectId = session.getSubjectId();
    report.totalScore = session.getTotalScore();
    report.maxScore = session.getMaxPossibleScore();
    report.percentage = (report.maxScore > 0)
        ? (static_cast<double>(report.totalScore) / report.maxScore) * 100.0
        : 0.0;

    // Initialize difficulty score maps
    report.scoreByDifficulty[Difficulty::EASY] = 0;
    report.scoreByDifficulty[Difficulty::MEDIUM] = 0;
    report.scoreByDifficulty[Difficulty::HARD] = 0;
    report.maxByDifficulty[Difficulty::EASY] = 0;
    report.maxByDifficulty[Difficulty::MEDIUM] = 0;
    report.maxByDifficulty[Difficulty::HARD] = 0;

    // Track topic performance: topic -> {correct, total}
    std::map<std::string, std::pair<int, int>> topicStats;

    for (const auto& rec : session.getRecords()) {
        Difficulty diff = rec.question->getDifficulty();
        report.scoreByDifficulty[diff] += rec.pointsEarned;
        report.maxByDifficulty[diff] += rec.maxPoints;

        const std::string& topic = rec.question->getTopic();
        topicStats[topic].second++;
        if (rec.isCorrect) {
            topicStats[topic].first++;
        }
    }

    // Determine strongest and weakest topic
    double bestAccuracy = -1.0;
    double worstAccuracy = 200.0;
    std::string bestTopic = "General";
    std::string worstTopic = "General";

    for (const auto& entry : topicStats) {
        int correct = entry.second.first;
        int total = entry.second.second;
        double acc = (total > 0) ? (static_cast<double>(correct) / total) : 0.0;

        if (acc > bestAccuracy) {
            bestAccuracy = acc;
            bestTopic = entry.first;
        }
        if (acc < worstAccuracy) {
            worstAccuracy = acc;
            worstTopic = entry.first;
        }
    }

    report.strongestTopic = bestTopic;
    report.weakestTopic = worstTopic;

    // Generate 2-3 targeted study suggestions
    if (report.percentage >= 80.0) {
        report.suggestions.push_back("Outstanding command of core principles! Proceed directly to Hard-tier coding challenges.");
        report.suggestions.push_back("Practice explaining system architecture and time-complexity trade-offs verbally under strict time limits.");
    } else if (report.percentage >= 50.0) {
        report.suggestions.push_back("Solid baseline knowledge. Focus revision on your weakest area: '" + worstTopic + "'.");
        report.suggestions.push_back("Review Medium-level questions and optimize your response latency to avoid timeout penalties.");
        report.suggestions.push_back("Solidify fundamentals in '" + bestTopic + "' and practice real interview mock simulations.");
    } else {
        report.suggestions.push_back("Foundational review needed for '" + worstTopic + "' and '" + session.getSubjectId() + "'.");
        report.suggestions.push_back("Work through Easy-tier practice questions before attempting timed mock interviews.");
        report.suggestions.push_back("Study textbook definitions, memory layouts, and syntax nuances for multiple-choice questions.");
    }

    report.aiEvaluation = "Rule-based engine assessment completed.";
    return report;
}
