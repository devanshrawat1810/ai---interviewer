#pragma once

#include "db/IScoreboardRepository.h"
#include <mutex>

// OOP Concept: Interface Implementation & Polymorphism
// JSON repository storing challenge scoreboards in data/scoreboards/<challenge-id>.json.
class JsonScoreboardRepository : public IScoreboardRepository {
private:
    std::string directoryPath_;
    mutable std::mutex mutex_;

    std::string getFilePath(const std::string& challengeId) const;

public:
    explicit JsonScoreboardRepository(std::string directoryPath = "data/scoreboards");
    ~JsonScoreboardRepository() override = default;

    void recordSolve(const ScoreboardEntry& entry) override;
    std::vector<ScoreboardEntry> getTopSolvers(const std::string& challengeId, int limit = 10) override;
    std::vector<ScoreboardEntry> getFastestSolvers(const std::string& challengeId, int limit = 10) override;
    std::vector<ScoreboardEntry> getAllForChallenge(const std::string& challengeId) override;
};
