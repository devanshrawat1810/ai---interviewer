#include <gtest/gtest.h>
#include "interview/DailyChallengeService.h"
#include "core/FakeClock.h"
#include "core/MCQQuestion.h"
#include "core/Student.h"
#include "db/JsonDailyChallengeRepository.h"
#include "db/JsonQuestionRepository.h"
#include <filesystem>
#include <set>

namespace fs = std::filesystem;

class DailyChallengeTest : public ::testing::Test {
protected:
    std::string testDailyPath = "temp_test_daily.json";
    std::string testQuestionsDir = "temp_test_daily_q";

    std::unique_ptr<JsonDailyChallengeRepository> dailyRepo;
    std::unique_ptr<JsonQuestionRepository> questionRepo;
    std::unique_ptr<FakeClock> clock;
    std::unique_ptr<DailyChallengeService> dailyService;

    void SetUp() override {
        cleanup();
        dailyRepo = std::make_unique<JsonDailyChallengeRepository>(testDailyPath);
        questionRepo = std::make_unique<JsonQuestionRepository>(testQuestionsDir);
        clock = std::make_unique<FakeClock>("2026-10-11");

        // Seed 40 distinct questions
        for (int i = 1; i <= 40; ++i) {
            std::string qId = "q_pool_" + std::to_string(i);
            auto q = std::make_shared<MCQQuestion>(
                qId, "CSE", "topic", Difficulty::EASY, "Question #" + std::to_string(i),
                "Exp", std::vector<std::string>{"A", "B"}, "A");
            questionRepo->save(*q);
        }

        dailyService = std::make_unique<DailyChallengeService>(*dailyRepo, *questionRepo, *clock);
    }

    void TearDown() override {
        cleanup();
    }

    void cleanup() {
        try {
            if (fs::exists(testDailyPath)) fs::remove(testDailyPath);
            if (fs::exists(testQuestionsDir)) fs::remove_all(testQuestionsDir);
        } catch (...) {}
    }
};

TEST_F(DailyChallengeTest, SameQuestionForSameDateDeterministic) {
    auto q1 = dailyService->getDailyQuestion("CSE");
    auto q2 = dailyService->getDailyQuestion("CSE");

    ASSERT_NE(q1, nullptr);
    ASSERT_NE(q2, nullptr);
    // Everyone on the same day gets the exact same question
    EXPECT_EQ(q1->getId(), q2->getId());
}

TEST_F(DailyChallengeTest, NoRepeatWithin30Days) {
    std::set<std::string> usedQuestionIds;

    // Simulate 30 consecutive calendar days
    for (int day = 0; day < 30; ++day) {
        auto q = dailyService->getDailyQuestion("CSE");
        ASSERT_NE(q, nullptr);

        // Verify this question has not been repeated in the 30-day window
        EXPECT_EQ(usedQuestionIds.count(q->getId()), 0u)
            << "Question " << q->getId() << " was repeated on day " << day << " (" << clock->getCurrentDate() << ")";

        usedQuestionIds.insert(q->getId());
        clock->advanceDays(1);
    }

    EXPECT_EQ(usedQuestionIds.size(), 30u);
}

TEST_F(DailyChallengeTest, CompletionAwardsBonusPoints) {
    Student student("s_daily", "Tester", "test@college.edu", "hash", "salt", "CSE");
    int initialScore = student.getTotalScore();

    bool completed = dailyService->completeDailyChallenge(student, "CSE", true, 50);
    EXPECT_TRUE(completed);
    EXPECT_EQ(student.getTotalScore(), initialScore + 50);
    EXPECT_EQ(student.getDailyChallengesCompleted(), 1);
}
