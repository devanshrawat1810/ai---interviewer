#include <gtest/gtest.h>
#include "core/Student.h"
#include "core/BadgeRule.h"

TEST(ProgressionBadgeTest, LevelUnlockingRules) {
    Student student("s1", "Alice", "alice@college.edu", "hash", "salt", "CSE");

    // Initially only EASY is unlocked for subject "cpp_oop"
    EXPECT_EQ(student.getUnlockedLevel("cpp_oop"), Difficulty::EASY);
    EXPECT_TRUE(student.isLevelUnlocked("cpp_oop", Difficulty::EASY));
    EXPECT_FALSE(student.isLevelUnlocked("cpp_oop", Difficulty::MEDIUM));
    EXPECT_FALSE(student.isLevelUnlocked("cpp_oop", Difficulty::HARD));

    // Solve 1 Easy challenge: still only Easy unlocked
    student.recordChallengeSolve("c_e1", "cpp_oop", Difficulty::EASY, 50);
    EXPECT_FALSE(student.isLevelUnlocked("cpp_oop", Difficulty::MEDIUM));

    // Solve 2nd Easy challenge: still only Easy unlocked
    student.recordChallengeSolve("c_e2", "cpp_oop", Difficulty::EASY, 50);
    EXPECT_FALSE(student.isLevelUnlocked("cpp_oop", Difficulty::MEDIUM));

    // Solve 3rd Easy challenge: MEDIUM UNLOCKED!
    student.recordChallengeSolve("c_e3", "cpp_oop", Difficulty::EASY, 50);
    EXPECT_TRUE(student.isLevelUnlocked("cpp_oop", Difficulty::MEDIUM));
    EXPECT_FALSE(student.isLevelUnlocked("cpp_oop", Difficulty::HARD));

    // Solve 3 Medium challenges: HARD UNLOCKED!
    student.recordChallengeSolve("c_m1", "cpp_oop", Difficulty::MEDIUM, 100);
    student.recordChallengeSolve("c_m2", "cpp_oop", Difficulty::MEDIUM, 100);
    student.recordChallengeSolve("c_m3", "cpp_oop", Difficulty::MEDIUM, 100);
    EXPECT_TRUE(student.isLevelUnlocked("cpp_oop", Difficulty::HARD));
}

TEST(ProgressionBadgeTest, BadgeRuleEvaluation) {
    Student student("s2", "Bob", "bob@college.edu", "hash", "salt", "CSE");
    BadgeEvaluator evaluator;

    // No badges initially
    auto badges0 = evaluator.evaluateNewBadges(student);
    EXPECT_TRUE(badges0.empty());

    // 1st solve triggers "First Solve" badge
    student.recordChallengeSolve("c1", "cpp_oop", Difficulty::EASY, 50);
    auto badges1 = evaluator.evaluateNewBadges(student);
    ASSERT_EQ(badges1.size(), 1u);
    EXPECT_EQ(badges1[0], "First Solve");
    student.addBadge("First Solve");

    // Re-evaluating does not award duplicate "First Solve"
    auto badges1_repeat = evaluator.evaluateNewBadges(student);
    EXPECT_TRUE(badges1_repeat.empty());

    // Reaching 7-day streak awards "7-Day Streak"
    student.setStreak(7);
    auto badges2 = evaluator.evaluateNewBadges(student);
    ASSERT_EQ(badges2.size(), 1u);
    EXPECT_EQ(badges2[0], "7-Day Streak");
    student.addBadge("7-Day Streak");

    // Solving a Hard challenge awards "Hard Master"
    student.recordChallengeSolve("c_h1", "cpp_oop", Difficulty::HARD, 150);
    auto badges3 = evaluator.evaluateNewBadges(student);
    ASSERT_EQ(badges3.size(), 1u);
    EXPECT_EQ(badges3[0], "Hard Master");
}
