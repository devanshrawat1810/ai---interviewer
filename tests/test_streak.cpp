#include <gtest/gtest.h>
#include "interview/StreakService.h"
#include "core/FakeClock.h"
#include "core/Student.h"
#include "db/JsonUserRepository.h"
#include <filesystem>

namespace fs = std::filesystem;

class StreakTest : public ::testing::Test {
protected:
    std::string testUsersPath = "temp_test_streak_users.json";
    std::unique_ptr<JsonUserRepository> userRepo;
    std::unique_ptr<FakeClock> clock;
    std::unique_ptr<StreakService> streakService;

    void SetUp() override {
        cleanup();
        userRepo = std::make_unique<JsonUserRepository>(testUsersPath);
        clock = std::make_unique<FakeClock>("2026-10-01");
        streakService = std::make_unique<StreakService>(*userRepo, *clock);
    }

    void TearDown() override {
        cleanup();
    }

    void cleanup() {
        try {
            if (fs::exists(testUsersPath)) fs::remove(testUsersPath);
        } catch (...) {}
    }
};

TEST_F(StreakTest, ContinueStreakOnConsecutiveDays) {
    Student student("s_str", "Streak Runner", "runner@college.edu", "hash", "salt", "CSE");
    userRepo->save(student);

    // Day 1: 2026-10-01
    streakService->recordActivity(student);
    EXPECT_EQ(student.getStreak(), 1);
    EXPECT_EQ(student.getLongestStreak(), 1);

    // Same day activity does not double count
    streakService->recordActivity(student);
    EXPECT_EQ(student.getStreak(), 1);

    // Day 2: 2026-10-02 (consecutive)
    clock->advanceDays(1);
    streakService->recordActivity(student);
    EXPECT_EQ(student.getStreak(), 2);
    EXPECT_EQ(student.getLongestStreak(), 2);

    // Day 3: 2026-10-03 (consecutive)
    clock->advanceDays(1);
    streakService->recordActivity(student);
    EXPECT_EQ(student.getStreak(), 3);
    EXPECT_EQ(student.getLongestStreak(), 3);
}

TEST_F(StreakTest, ResetStreakAfterMissedDayAndTrackLongest) {
    Student student("s_str2", "Streak Resetter", "reset@college.edu", "hash", "salt", "CSE");
    userRepo->save(student);

    // Build 3-day streak: 2026-10-01 to 2026-10-03
    streakService->recordActivity(student);
    clock->advanceDays(1);
    streakService->recordActivity(student);
    clock->advanceDays(1);
    streakService->recordActivity(student);

    EXPECT_EQ(student.getStreak(), 3);
    EXPECT_EQ(student.getLongestStreak(), 3);

    // Skip 2 full days -> Advance to 2026-10-05 (missed 2026-10-04)
    clock->advanceDays(2);
    streakService->recordActivity(student);

    // Streak resets to 1, but longest streak remains 3!
    EXPECT_EQ(student.getStreak(), 1);
    EXPECT_EQ(student.getLongestStreak(), 3);
}

TEST_F(StreakTest, StreakRiskDetection) {
    Student student("s_str3", "At Risk Student", "risk@college.edu", "hash", "salt", "CSE");
    userRepo->save(student);

    // Day 1 activity
    streakService->recordActivity(student);
    EXPECT_FALSE(streakService->isStreakAtRisk(student));

    // Next day arrives (no activity recorded yet today) -> Streak is at risk!
    clock->advanceDays(1);
    EXPECT_TRUE(streakService->isStreakAtRisk(student));

    // Once activity recorded today -> no longer at risk
    streakService->recordActivity(student);
    EXPECT_FALSE(streakService->isStreakAtRisk(student));
}

TEST_F(StreakTest, MonthCalendarRendering) {
    Student student("s_cal", "Calendar Student", "cal@college.edu", "hash", "salt", "CSE");
    userRepo->save(student);

    // Record activity on day 1 and day 2
    streakService->recordActivity(student);
    clock->advanceDays(1);
    streakService->recordActivity(student);

    std::string cal = streakService->renderStreakCalendar(student, 2026, 10);
    EXPECT_NE(cal.find("STREAK CALENDAR"), std::string::npos);
    EXPECT_NE(cal.find("Sun  Mon  Tue  Wed  Thu  Fri  Sat"), std::string::npos);
    // Active days should be marked
    EXPECT_NE(cal.find("[01]"), std::string::npos);
    EXPECT_NE(cal.find("[02]"), std::string::npos);
}
