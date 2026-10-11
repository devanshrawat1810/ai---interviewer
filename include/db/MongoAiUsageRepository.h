#pragma once

#include "db/IAiUsageRepository.h"
#include <map>
#include <mutex>

// OOP Concept: Interface Implementation & Polymorphism
// MongoDB storage implementation for user AI call quotas and review counters.
class MongoAiUsageRepository : public IAiUsageRepository {
private:
    mutable std::mutex mutex_;
    std::map<std::string, std::map<std::string, int>> inMemoryUsageFallback_;
    std::map<std::string, std::map<std::string, int>> inMemoryReviewFallback_;

public:
    MongoAiUsageRepository() = default;
    ~MongoAiUsageRepository() override = default;

    int getUsage(const std::string& userId, const std::string& date) override;
    void incrementUsage(const std::string& userId, const std::string& date, int count = 1) override;

    int getReviewUsage(const std::string& userId, const std::string& date) override;
    void incrementReviewUsage(const std::string& userId, const std::string& date) override;
};
