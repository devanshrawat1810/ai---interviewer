#pragma once

#include <string>

struct LeaderboardEntry {
    std::string userId;
    std::string name;
    std::string course;
    std::string college;
    std::string branch;
    int challengesSolved{0};
    int totalScore{0};
    int streak{0};
    std::string earliestSolveTime;
};

enum class LeaderboardScope {
    GLOBAL,
    BY_COLLEGE,
    BY_BRANCH,
    BY_SUBJECT
};

enum class TimeWindow {
    ALL_TIME,
    THIS_WEEK,
    THIS_MONTH
};

// OOP Concept: Strategy Pattern
// Abstract strategy decoupling leaderboard sorting criteria from display logic.
class IRankingStrategy {
public:
    virtual ~IRankingStrategy() = default;
    virtual bool isHigherRank(const LeaderboardEntry& a, const LeaderboardEntry& b) const = 0;
    virtual std::string getName() const = 0;
};

class RankByTotalScore : public IRankingStrategy {
public:
    bool isHigherRank(const LeaderboardEntry& a, const LeaderboardEntry& b) const override;
    std::string getName() const override;
};

class RankBySolvedCount : public IRankingStrategy {
public:
    bool isHigherRank(const LeaderboardEntry& a, const LeaderboardEntry& b) const override;
    std::string getName() const override;
};

class RankByStreak : public IRankingStrategy {
public:
    bool isHigherRank(const LeaderboardEntry& a, const LeaderboardEntry& b) const override;
    std::string getName() const override;
};
