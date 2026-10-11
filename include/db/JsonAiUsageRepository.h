#pragma once

#include "db/IAiUsageRepository.h"
#include <string>
#include <map>
#include <mutex>

// OOP Concept: Concrete Repository Implementation
// Persists user AI usage metrics and review counts into local JSON storage.
class JsonAiUsageRepository : public IAiUsageRepository {
private:
    std::string filePath_;
    mutable std::mutex mutex_;

    std::map<std::string, std::map<std::string, int>> usageMap_;        // user -> date -> count
    std::map<std::string, std::map<std::string, int>> reviewUsageMap_;  // user -> date -> reviews

    void load();
    void save();

public:
    explicit JsonAiUsageRepository(std::string filePath = "data/ai_usage.json");
    ~JsonAiUsageRepository() override = default;

    int getUsage(const std::string& userId, const std::string& date) override;
    void incrementUsage(const std::string& userId, const std::string& date, int count = 1) override;

    int getReviewUsage(const std::string& userId, const std::string& date) override;
    void incrementReviewUsage(const std::string& userId, const std::string& date) override;
};
