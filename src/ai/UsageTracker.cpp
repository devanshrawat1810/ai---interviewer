#include "ai/UsageTracker.h"
#include "db/JsonAiUsageRepository.h"
#include "core/SystemClock.h"
#include <cstdlib>

static int getEnvDailyLimit(int defaultVal) {
#if defined(_MSC_VER)
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "AI_DAILY_LIMIT") == 0 && buf != nullptr) {
        std::string val(buf);
        free(buf);
        try { return std::stoi(val); } catch (...) {}
    }
    return defaultVal;
#else
    const char* val = std::getenv("AI_DAILY_LIMIT");
    if (val) {
        try { return std::stoi(val); } catch (...) {}
    }
    return defaultVal;
#endif
}

UsageTracker::UsageTracker(std::shared_ptr<IAiUsageRepository> repo,
                           std::shared_ptr<IClock> clock,
                           int dailyLimit,
                           int dailyReviewLimit)
    : repository_(repo ? std::move(repo) : std::make_shared<JsonAiUsageRepository>()),
      clock_(clock ? std::move(clock) : std::make_shared<SystemClock>()),
      dailyLimit_(dailyLimit > 0 ? dailyLimit : getEnvDailyLimit(30)),
      dailyReviewLimit_(dailyReviewLimit) {}

bool UsageTracker::canMakeAiCall(const std::string& userId) const {
    if (userId.empty()) return false;
    std::string today = clock_->getCurrentDate();
    int used = repository_->getUsage(userId, today);
    return used < dailyLimit_;
}

void UsageTracker::recordAiCall(const std::string& userId, int count) {
    if (userId.empty()) return;
    std::string today = clock_->getCurrentDate();
    repository_->incrementUsage(userId, today, count);
}

int UsageTracker::getRemainingCalls(const std::string& userId) const {
    if (userId.empty()) return 0;
    std::string today = clock_->getCurrentDate();
    int used = repository_->getUsage(userId, today);
    int remaining = dailyLimit_ - used;
    return remaining > 0 ? remaining : 0;
}

int UsageTracker::getTodayUsage(const std::string& userId) const {
    if (userId.empty()) return 0;
    std::string today = clock_->getCurrentDate();
    return repository_->getUsage(userId, today);
}

bool UsageTracker::canReviewAnswer(const std::string& userId) const {
    if (userId.empty()) return false;
    std::string today = clock_->getCurrentDate();
    int used = repository_->getReviewUsage(userId, today);
    return used < dailyReviewLimit_;
}

void UsageTracker::recordReview(const std::string& userId) {
    if (userId.empty()) return;
    std::string today = clock_->getCurrentDate();
    repository_->incrementReviewUsage(userId, today);
}

int UsageTracker::getRemainingReviews(const std::string& userId) const {
    if (userId.empty()) return 0;
    std::string today = clock_->getCurrentDate();
    int used = repository_->getReviewUsage(userId, today);
    int remaining = dailyReviewLimit_ - used;
    return remaining > 0 ? remaining : 0;
}
