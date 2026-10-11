#include <gtest/gtest.h>
#include "interview/LinearRampStrategy.h"
#include "interview/AdaptiveStrategy.h"

TEST(StrategyTest, LinearRampProgression) {
    LinearRampStrategy strategy;
    std::vector<bool> dummyHistory;

    // Q1-3 should be EASY
    EXPECT_EQ(strategy.getNextDifficulty(1, dummyHistory), Difficulty::EASY);
    EXPECT_EQ(strategy.getNextDifficulty(2, dummyHistory), Difficulty::EASY);
    EXPECT_EQ(strategy.getNextDifficulty(3, dummyHistory), Difficulty::EASY);

    // Q4-7 should be MEDIUM
    EXPECT_EQ(strategy.getNextDifficulty(4, dummyHistory), Difficulty::MEDIUM);
    EXPECT_EQ(strategy.getNextDifficulty(5, dummyHistory), Difficulty::MEDIUM);
    EXPECT_EQ(strategy.getNextDifficulty(6, dummyHistory), Difficulty::MEDIUM);
    EXPECT_EQ(strategy.getNextDifficulty(7, dummyHistory), Difficulty::MEDIUM);

    // Q8-10 should be HARD
    EXPECT_EQ(strategy.getNextDifficulty(8, dummyHistory), Difficulty::HARD);
    EXPECT_EQ(strategy.getNextDifficulty(9, dummyHistory), Difficulty::HARD);
    EXPECT_EQ(strategy.getNextDifficulty(10, dummyHistory), Difficulty::HARD);
}

TEST(StrategyTest, AdaptiveStrategyLevelUpAndDown) {
    AdaptiveStrategy strategy(Difficulty::EASY);

    // Initial state: EASY
    EXPECT_EQ(strategy.getCurrentDifficulty(), Difficulty::EASY);

    // 1 correct answer: still EASY
    strategy.getNextDifficulty(1, {true});
    EXPECT_EQ(strategy.getCurrentDifficulty(), Difficulty::EASY);

    // 2 consecutive correct answers: levels up to MEDIUM
    strategy.getNextDifficulty(2, {true, true});
    EXPECT_EQ(strategy.getCurrentDifficulty(), Difficulty::MEDIUM);

    // Another 2 consecutive correct answers: levels up to HARD
    strategy.getNextDifficulty(3, {true, true, true});
    EXPECT_EQ(strategy.getCurrentDifficulty(), Difficulty::MEDIUM);
    strategy.getNextDifficulty(4, {true, true, true, true});
    EXPECT_EQ(strategy.getCurrentDifficulty(), Difficulty::HARD);

    // 1 wrong answer: stays HARD
    strategy.getNextDifficulty(5, {true, true, true, true, false});
    EXPECT_EQ(strategy.getCurrentDifficulty(), Difficulty::HARD);

    // 2 consecutive wrong answers: levels down to MEDIUM
    strategy.getNextDifficulty(6, {true, true, true, true, false, false});
    EXPECT_EQ(strategy.getCurrentDifficulty(), Difficulty::MEDIUM);

    // Another 2 consecutive wrong answers: levels down to EASY
    strategy.getNextDifficulty(7, {true, true, true, true, false, false, false, false});
    EXPECT_EQ(strategy.getCurrentDifficulty(), Difficulty::EASY);
}

#include "ui/RankingStrategy.h"

TEST(StrategyTest, RankingStrategyScoreComparison) {
    RankByTotalScore strategy;
    EXPECT_EQ(strategy.getName(), "Total Score");

    LeaderboardEntry a{"u1", "Alice", "CSE", "MIT", "CS", 5, 250, 3, "2026-10-01"};
    LeaderboardEntry b{"u2", "Bob", "CSE", "MIT", "CS", 10, 200, 5, "2026-10-01"};

    // Alice has higher totalScore (250 vs 200), should rank higher
    EXPECT_TRUE(strategy.isHigherRank(a, b));
    EXPECT_FALSE(strategy.isHigherRank(b, a));
}

TEST(StrategyTest, RankingStrategySolvedCountComparison) {
    RankBySolvedCount strategy;
    EXPECT_EQ(strategy.getName(), "Challenges Solved");

    LeaderboardEntry a{"u1", "Alice", "CSE", "MIT", "CS", 5, 300, 3, "2026-10-01"};
    LeaderboardEntry b{"u2", "Bob", "CSE", "MIT", "CS", 10, 200, 3, "2026-10-01"};

    // Bob has more solved challenges (10 vs 5), should rank higher
    EXPECT_TRUE(strategy.isHigherRank(b, a));
    EXPECT_FALSE(strategy.isHigherRank(a, b));
}

TEST(StrategyTest, RankingStrategyStreakComparison) {
    RankByStreak strategy;
    EXPECT_EQ(strategy.getName(), "Active Streak");

    LeaderboardEntry a{"u1", "Alice", "CSE", "MIT", "CS", 5, 300, 15, "2026-10-01"};
    LeaderboardEntry b{"u2", "Bob", "CSE", "MIT", "CS", 10, 500, 3, "2026-10-01"};

    // Alice has higher streak (15 vs 3), should rank higher
    EXPECT_TRUE(strategy.isHigherRank(a, b));
    EXPECT_FALSE(strategy.isHigherRank(b, a));
}

