#include "ui/RankingStrategy.h"

// OOP Concept: Strategy Pattern Implementation
// Implements ranking comparisons and tie-breaking algorithms.

bool RankByTotalScore::isHigherRank(const LeaderboardEntry& a, const LeaderboardEntry& b) const {
    if (a.totalScore != b.totalScore) return a.totalScore > b.totalScore;
    if (a.challengesSolved != b.challengesSolved) return a.challengesSolved > b.challengesSolved;
    if (a.streak != b.streak) return a.streak > b.streak;
    return a.earliestSolveTime < b.earliestSolveTime;
}

std::string RankByTotalScore::getName() const {
    return "Total Score";
}

bool RankBySolvedCount::isHigherRank(const LeaderboardEntry& a, const LeaderboardEntry& b) const {
    if (a.challengesSolved != b.challengesSolved) return a.challengesSolved > b.challengesSolved;
    if (a.totalScore != b.totalScore) return a.totalScore > b.totalScore;
    if (a.streak != b.streak) return a.streak > b.streak;
    return a.earliestSolveTime < b.earliestSolveTime;
}

std::string RankBySolvedCount::getName() const {
    return "Challenges Solved";
}

bool RankByStreak::isHigherRank(const LeaderboardEntry& a, const LeaderboardEntry& b) const {
    if (a.streak != b.streak) return a.streak > b.streak;
    if (a.totalScore != b.totalScore) return a.totalScore > b.totalScore;
    if (a.challengesSolved != b.challengesSolved) return a.challengesSolved > b.challengesSolved;
    return a.earliestSolveTime < b.earliestSolveTime;
}

std::string RankByStreak::getName() const {
    return "Active Streak";
}
