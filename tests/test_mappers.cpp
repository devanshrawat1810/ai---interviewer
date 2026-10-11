#include <gtest/gtest.h>
#include "db/UserMapper.h"
#include "db/QuestionMapper.h"
#include "db/AttemptMapper.h"
#include "core/Student.h"
#include "core/MCQQuestion.h"
#include "core/TextQuestion.h"
#include "core/CodingQuestion.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/json.hpp>
#endif

TEST(MapperTest, UserJsonRoundTrip) {
    Student student("u100", "Charlie", "charlie@college.edu", "hash123", "salt123", "CSE", "MIT", "CS", 4, 7, "2026-10-09");
    student.addBadge("First Solve");
    student.addBookmark("cpp_e1");
    student.addActivityDate("2026-10-09");
    student.recordChallengeSolve("c1", "cpp_oop", Difficulty::EASY, 50);

    nlohmann::json j = UserMapper::toJson(student);
    auto restored = UserMapper::fromJson(j);

    ASSERT_NE(restored, nullptr);
    EXPECT_EQ(restored->getId(), "u100");
    EXPECT_EQ(restored->getName(), "Charlie");
    EXPECT_EQ(restored->getEmail(), "charlie@college.edu");
    EXPECT_EQ(restored->getCollege(), "MIT");
    EXPECT_EQ(restored->getBranch(), "CS");
    EXPECT_EQ(restored->getStreak(), 4);
    EXPECT_EQ(restored->getLongestStreak(), 7);
    EXPECT_TRUE(restored->hasBookmark("cpp_e1"));

    auto restoredStudent = std::dynamic_pointer_cast<Student>(restored);
    ASSERT_NE(restoredStudent, nullptr);
    EXPECT_TRUE(restoredStudent->hasBadge("First Solve"));
    EXPECT_EQ(restoredStudent->getTotalScore(), 50);
}

TEST(MapperTest, QuestionJsonRoundTripMCQ) {
    MCQQuestion mcq("q_mcq", "cpp_oop", "classes", Difficulty::EASY, "Question text", "Exp", {"A", "B"}, "A");
    nlohmann::json j = QuestionMapper::toJson(mcq);
    auto restored = QuestionMapper::fromJson(j);

    ASSERT_NE(restored, nullptr);
    EXPECT_EQ(restored->getId(), "q_mcq");
    EXPECT_EQ(restored->getType(), "mcq");
    EXPECT_TRUE(restored->evaluate("A"));
}

TEST(MapperTest, QuestionJsonRoundTripCoding) {
    std::vector<TestCase> cases = {{"5\n", "10", false}};
    CodingQuestion code("q_code", "cpp_oop", "math", Difficulty::HARD, "Double", "Exp", "code", cases, 150);
    nlohmann::json j = QuestionMapper::toJson(code);
    auto restored = QuestionMapper::fromJson(j);

    ASSERT_NE(restored, nullptr);
    EXPECT_EQ(restored->getId(), "q_code");
    EXPECT_EQ(restored->getType(), "coding");
    EXPECT_EQ(restored->getDifficulty(), Difficulty::HARD);
}

TEST(MapperTest, AttemptJsonRoundTrip) {
    QuestionAttempt att;
    att.id = "att_99";
    att.userId = "user_1";
    att.questionId = "q_1";
    att.subject = "cpp_oop";
    att.difficulty = Difficulty::MEDIUM;
    att.userResponse = "my answer";
    att.isCorrect = true;
    att.timeTakenSeconds = 45;
    att.pointsEarned = 2;
    att.date = "2026-10-09";

    nlohmann::json j = AttemptMapper::toJson(att);
    QuestionAttempt restored = AttemptMapper::fromJson(j);

    EXPECT_EQ(restored.id, att.id);
    EXPECT_EQ(restored.userId, att.userId);
    EXPECT_EQ(restored.pointsEarned, 2);
    EXPECT_TRUE(restored.isCorrect);
}

#ifdef AI_INTERVIEWER_HAS_MONGODB
TEST(MapperTest, UserBsonRoundTrip) {
    Student student("u_bson", "Diana", "diana@college.edu", "hashBson", "saltBson", "ECE", "Stanford", "ECE", 7, 10, "2026-10-09");
    student.addBadge("7-Day Streak");

    auto doc = UserMapper::toBson(student);
    auto restored = UserMapper::fromBson(doc.view());

    ASSERT_NE(restored, nullptr);
    EXPECT_EQ(restored->getId(), "u_bson");
    EXPECT_EQ(restored->getEmail(), "diana@college.edu");
    EXPECT_EQ(restored->getCollege(), "Stanford");
    EXPECT_EQ(restored->getBranch(), "ECE");

    auto s = std::dynamic_pointer_cast<Student>(restored);
    ASSERT_NE(s, nullptr);
    EXPECT_TRUE(s->hasBadge("7-Day Streak"));
}
#endif
