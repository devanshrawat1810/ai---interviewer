#include <gtest/gtest.h>
#include "core/MCQQuestion.h"
#include "core/TextQuestion.h"
#include "core/Difficulty.h"

TEST(QuestionTest, MCQEvaluationLetterAndText) {
    MCQQuestion q(
        "q1", "cpp_oop", "classes", Difficulty::EASY,
        "Which access specifier restricts visibility?",
        "Private restricts visibility.",
        {"public", "private", "protected"},
        "private"
    );

    // Option index matching: Option B is "private"
    EXPECT_TRUE(q.evaluate("B"));
    EXPECT_TRUE(q.evaluate("b"));

    // Direct text matching
    EXPECT_TRUE(q.evaluate("private"));
    EXPECT_TRUE(q.evaluate("PRIVATE"));
    EXPECT_TRUE(q.evaluate("  private  "));

    // Incorrect answers
    EXPECT_FALSE(q.evaluate("A"));
    EXPECT_FALSE(q.evaluate("public"));
    EXPECT_FALSE(q.evaluate(""));
    EXPECT_FALSE(q.evaluate("Z"));
}

TEST(QuestionTest, TextQuestionKeywordEvaluation) {
    TextQuestion q(
        "q2", "cpp_oop", "polymorphism", Difficulty::MEDIUM,
        "Explain virtual destructor necessity.",
        "Prevents memory leaks when deleting via base pointer.",
        "Prevents memory leaks in derived class.",
        {"virtual", "destructor", "leak", "derived"},
        0.5 // Threshold: at least 50% (2 of 4) keywords
    );

    // Meets threshold (matches "virtual", "destructor", "leak" -> 3/4 = 75%)
    EXPECT_TRUE(q.evaluate("A virtual destructor prevents a memory leak."));

    // Meets threshold (matches "derived", "leak" -> 2/4 = 50%)
    EXPECT_TRUE(q.evaluate("It cleans derived resources avoiding a leak."));

    // Fails threshold (matches only "destructor" -> 1/4 = 25%)
    EXPECT_FALSE(q.evaluate("It is just a normal destructor."));

    // Empty response fails
    EXPECT_FALSE(q.evaluate(""));
}
