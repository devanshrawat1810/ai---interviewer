#include <gtest/gtest.h>
#include "interview/InterviewSession.h"
#include "interview/LinearRampStrategy.h"
#include "core/MCQQuestion.h"

TEST(InterviewTest, WeightedScoreCalculation) {
    auto qEasy = std::make_shared<MCQQuestion>(
        "q_e", "cpp", "basics", Difficulty::EASY, "Easy Q", "Exp", std::vector<std::string>{"A", "B"}, "A");
    auto qMed = std::make_shared<MCQQuestion>(
        "q_m", "cpp", "basics", Difficulty::MEDIUM, "Med Q", "Exp", std::vector<std::string>{"A", "B"}, "A");
    auto qHard = std::make_shared<MCQQuestion>(
        "q_h", "cpp", "basics", Difficulty::HARD, "Hard Q", "Exp", std::vector<std::string>{"A", "B"}, "A");

    // Weights: Easy 1, Medium 2, Hard 3
    EXPECT_EQ(InterviewSession::getWeightForDifficulty(Difficulty::EASY), 1);
    EXPECT_EQ(InterviewSession::getWeightForDifficulty(Difficulty::MEDIUM), 2);
    EXPECT_EQ(InterviewSession::getWeightForDifficulty(Difficulty::HARD), 3);

    // Time limits: Easy 60s, Medium 120s, Hard 240s
    EXPECT_EQ(InterviewSession::getTimeLimitForDifficulty(Difficulty::EASY), 60);
    EXPECT_EQ(InterviewSession::getTimeLimitForDifficulty(Difficulty::MEDIUM), 120);
    EXPECT_EQ(InterviewSession::getTimeLimitForDifficulty(Difficulty::HARD), 240);

    std::vector<std::shared_ptr<Question>> pool = {qEasy, qMed, qHard};
    auto strategy = std::make_unique<LinearRampStrategy>();

    InterviewSession session("sess_test", "user1", "cpp", "mock", std::move(strategy), pool, 3);
    session.start();

    // 1. Submit correct answer on Easy in 15 seconds (within 60s) -> 1 pt
    bool res1 = session.submitAnswer(qEasy, "A", 15);
    EXPECT_TRUE(res1);
    EXPECT_EQ(session.getTotalScore(), 1);

    // 2. Submit correct answer on Medium in 30 seconds (within 120s) -> 2 pts
    bool res2 = session.submitAnswer(qMed, "A", 30);
    EXPECT_TRUE(res2);
    EXPECT_EQ(session.getTotalScore(), 3); // 1 + 2

    // 3. Submit correct answer on Hard but TIMED OUT (300s > 240s) -> 0 pts
    bool res3 = session.submitAnswer(qHard, "A", 300);
    EXPECT_FALSE(res3);
    EXPECT_EQ(session.getTotalScore(), 3); // Still 3 because timed out

    EXPECT_TRUE(session.isCompleted());
}
