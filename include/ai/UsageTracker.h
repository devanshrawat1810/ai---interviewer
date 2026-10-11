#pragma once

#include "db/IAiUsageRepository.h"
#include "core/IClock.h"
#include <memory>
#include <string>

// OOP Concept: Encapsulation & Guard Pattern
// Tracks AI call quotas per student per day, enforces limits, and supports time-travel testing via IClock.
class UsageTracker {
private:
    std::shared_ptr<IAiUsageRepository> repository_;
    std::shared_ptr<IClock> clock_;
    int dailyLimit_{30};
    int dailyReviewLimit_{5};

public:
    UsageTracker(std::shared_ptr<IAiUsageRepository> repo = nullptr,
                 std::shared_ptr<IClock> clock = nullptr,
                 int dailyLimit = 30,
                 int dailyReviewLimit = 5);

    bool canMakeAiCall(const std::string& userId) const;
    void recordAiCall(const std::string& userId, int count = 1);
    int getRemainingCalls(const std::string& userId) const;
    int getTodayUsage(const std::string& userId) const;

    bool canReviewAnswer(const std::string& userId) const;
    void recordReview(const std::string& userId);
    int getRemainingReviews(const std::string& userId) const;

    int getDailyLimit() const noexcept { return dailyLimit_; }
    void setDailyLimit(int limit) noexcept { dailyLimit_ = limit; }
};
