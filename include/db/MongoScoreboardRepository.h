#pragma once

#include "db/IScoreboardRepository.h"

// OOP Concept: Interface Implementation & Polymorphism
// MongoDB challenge scoreboard repository satisfying IScoreboardRepository.
// Queries fastest solvers and highest scores using MongoDB sorting and indexing.
class MongoScoreboardRepository : public IScoreboardRepository {
public:
    MongoScoreboardRepository() = default;
    ~MongoScoreboardRepository() override = default;

    void recordSolve(const ScoreboardEntry& entry) override;
    std::vector<ScoreboardEntry> getTopSolvers(const std::string& challengeId, int limit = 10) override;
    std::vector<ScoreboardEntry> getFastestSolvers(const std::string& challengeId, int limit = 10) override;
    std::vector<ScoreboardEntry> getAllForChallenge(const std::string& challengeId) override;
};
