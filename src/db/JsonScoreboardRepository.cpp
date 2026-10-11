#include "db/JsonScoreboardRepository.h"
#include "db/ScoreboardMapper.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of per-challenge scoreboards stored in data/scoreboards/<challenge-id>.json.

JsonScoreboardRepository::JsonScoreboardRepository(std::string directoryPath)
    : directoryPath_(std::move(directoryPath)) {
    try {
        fs::create_directories(directoryPath_);
    } catch (...) {}
}

std::string JsonScoreboardRepository::getFilePath(const std::string& challengeId) const {
    return directoryPath_ + "/" + challengeId + ".json";
}

void JsonScoreboardRepository::recordSolve(const ScoreboardEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string path = getFilePath(entry.challengeId);

    nlohmann::json arr = nlohmann::json::array();
    if (fs::exists(path)) {
        std::ifstream in(path);
        if (in) {
            try {
                in >> arr;
            } catch (...) {
                arr = nlohmann::json::array();
            }
        }
    }

    if (!arr.is_array()) {
        arr = nlohmann::json::array();
    }

    // Check if user already on scoreboard; update if faster or higher score
    bool updated = false;
    for (auto& item : arr) {
        if (item.value("userId", "") == entry.userId) {
            int oldPoints = item.value("points", 0);
            int oldTime = item.value("executionTimeMs", 999999);
            if (entry.points > oldPoints || (entry.points == oldPoints && entry.executionTimeMs < oldTime)) {
                item = ScoreboardMapper::toJson(entry);
            }
            updated = true;
            break;
        }
    }

    if (!updated) {
        arr.push_back(ScoreboardMapper::toJson(entry));
    }

    std::ofstream out(path);
    out << arr.dump(2);
}

std::vector<ScoreboardEntry> JsonScoreboardRepository::getAllForChallenge(const std::string& challengeId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ScoreboardEntry> results;
    std::string path = getFilePath(challengeId);

    if (!fs::exists(path)) return results;

    std::ifstream in(path);
    if (!in) return results;

    nlohmann::json arr;
    try {
        in >> arr;
    } catch (...) {
        return results;
    }

    if (!arr.is_array()) return results;

    for (const auto& item : arr) {
        results.push_back(ScoreboardMapper::fromJson(item));
    }
    return results;
}

std::vector<ScoreboardEntry> JsonScoreboardRepository::getTopSolvers(const std::string& challengeId, int limit) {
    auto results = getAllForChallenge(challengeId);
    std::sort(results.begin(), results.end(), [](const ScoreboardEntry& a, const ScoreboardEntry& b) {
        if (a.points != b.points) {
            return a.points > b.points;
        }
        return a.executionTimeMs < b.executionTimeMs;
    });

    if (static_cast<int>(results.size()) > limit) {
        results.resize(static_cast<size_t>(limit));
    }
    return results;
}

std::vector<ScoreboardEntry> JsonScoreboardRepository::getFastestSolvers(const std::string& challengeId, int limit) {
    auto results = getAllForChallenge(challengeId);
    std::sort(results.begin(), results.end(), [](const ScoreboardEntry& a, const ScoreboardEntry& b) {
        if (a.executionTimeMs != b.executionTimeMs) {
            return a.executionTimeMs < b.executionTimeMs;
        }
        return a.points > b.points;
    });

    if (static_cast<int>(results.size()) > limit) {
        results.resize(static_cast<size_t>(limit));
    }
    return results;
}
