#pragma once

#include <string>
#include <vector>
#include <memory>

struct ScoreboardEntry {
    std::string challengeId;
    std::string userId;
    std::string userName;
    int points{0};
    int executionTimeMs{0};
    std::string solvedAt; // ISO 8601 timestamp
};

// OOP Concept: Dependency Inversion Principle
// Interface for per-challenge scoreboards tracking fastest solvers and top scores.
class IScoreboardRepository {
public:
    virtual ~IScoreboardRepository() = default;

    virtual void recordSolve(const ScoreboardEntry& entry) = 0;
    virtual std::vector<ScoreboardEntry> getTopSolvers(const std::string& challengeId, int limit = 10) = 0;
    virtual std::vector<ScoreboardEntry> getFastestSolvers(const std::string& challengeId, int limit = 10) = 0;
    virtual std::vector<ScoreboardEntry> getAllForChallenge(const std::string& challengeId) = 0;
};
