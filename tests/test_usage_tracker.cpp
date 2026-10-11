#include <gtest/gtest.h>
#include "ai/UsageTracker.h"
#include "db/JsonAiUsageRepository.h"
#include <memory>
#include <chrono>

// Test double for IClock to test day rollover
class TestClock : public IClock {
public:
    std::string currentDate;

    explicit TestClock(std::string d) : currentDate(std::move(d)) {}

    std::string getIsoDate() const override {
        return currentDate;
    }

    std::chrono::system_clock::time_point now() const override {
        return std::chrono::system_clock::now();
    }
};

TEST(UsageTrackerTest, DailyLimitEnforcedAndCountsDown) {
    auto repo = std::make_shared<JsonAiUsageRepository>("test_ai_usage.json");
    auto clock = std::make_shared<TestClock>("2026-10-11");
    UsageTracker tracker(repo, clock, 3); // limit = 3

    std::string user = "test_user_quota";

    EXPECT_TRUE(tracker.canMakeAiCall(user));
    EXPECT_EQ(tracker.getRemainingCalls(user), 3);

    tracker.recordAiCall(user);
    EXPECT_EQ(tracker.getRemainingCalls(user), 2);
    EXPECT_TRUE(tracker.canMakeAiCall(user));

    tracker.recordAiCall(user);
    EXPECT_EQ(tracker.getRemainingCalls(user), 1);
    EXPECT_TRUE(tracker.canMakeAiCall(user));

    tracker.recordAiCall(user);
    EXPECT_EQ(tracker.getRemainingCalls(user), 0);
    EXPECT_FALSE(tracker.canMakeAiCall(user)); // limit reached
}

TEST(UsageTrackerTest, QuotaResetsOnNewDay) {
    auto repo = std::make_shared<JsonAiUsageRepository>("test_ai_usage_rollover.json");
    auto clock = std::make_shared<TestClock>("2026-10-11");
    UsageTracker tracker(repo, clock, 2); // limit = 2

    std::string user = "test_user_rollover";

    tracker.recordAiCall(user);
    tracker.recordAiCall(user);
    EXPECT_FALSE(tracker.canMakeAiCall(user));

    // Fast-forward to the next day
    clock->currentDate = "2026-10-12";

    // Quota should be fully restored for the new day
    EXPECT_TRUE(tracker.canMakeAiCall(user));
    EXPECT_EQ(tracker.getRemainingCalls(user), 2);
}

TEST(UsageTrackerTest, AnswerReviewDailyLimitEnforced) {
    auto repo = std::make_shared<JsonAiUsageRepository>("test_ai_usage_reviews.json");
    auto clock = std::make_shared<TestClock>("2026-10-11");
    UsageTracker tracker(repo, clock, 30); // 5 reviews max per student per day

    std::string user = "test_user_reviews";

    EXPECT_EQ(tracker.getRemainingReviews(user), 5);
    EXPECT_TRUE(tracker.canReviewAnswer(user));

    for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(tracker.canReviewAnswer(user));
        tracker.recordReview(user);
    }

    EXPECT_EQ(tracker.getRemainingReviews(user), 0);
    EXPECT_FALSE(tracker.canReviewAnswer(user));

    // Rollover day
    clock->currentDate = "2026-10-12";
    EXPECT_TRUE(tracker.canReviewAnswer(user));
    EXPECT_EQ(tracker.getRemainingReviews(user), 5);
}
