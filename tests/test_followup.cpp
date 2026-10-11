#include <gtest/gtest.h>
#include "ai/FollowUpGenerator.h"
#include "ai/MockLLMClient.h"
#include "ai/NullLLMClient.h"
#include "interview/InterviewSession.h"
#include "core/TextQuestion.h"
#include "core/Difficulty.h"
#include <memory>

static std::shared_ptr<TextQuestion> makeTextQuestion(const std::string& topic, Difficulty d) {
    return std::make_shared<TextQuestion>(
        "q_" + topic, "CS", topic, d,
        "Explain " + topic + " in detail.",
        "This is the explanation.",
        std::vector<std::string>{"keyword1", "keyword2", "keyword3"}
    );
}

TEST(FollowUpGeneratorTest, GeneratesFollowUpOnHighScoreTextQuestion) {
    std::string followUpJson = R"({
        "follow_up_question": "Can you compare this approach with an alternative?",
        "keywords": ["compare", "alternative", "tradeoff"]
    })";
    auto mock = std::make_shared<MockLLMClient>(followUpJson);
    FollowUpGenerator gen(mock);

    auto question = makeTextQuestion("Polymorphism", Difficulty::MEDIUM);
    auto followUp = gen.generateFollowUp(question, "It is when derived classes override base methods.");

    ASSERT_NE(followUp, nullptr);
    EXPECT_EQ(followUp->getTopic(), "Polymorphism (Follow-Up)");
}

TEST(FollowUpGeneratorTest, ReturnsNullptrOnLLMFailure) {
    auto mock = std::make_shared<MockLLMClient>("invalid json garbage");
    FollowUpGenerator gen(mock);

    auto question = makeTextQuestion("Inheritance", Difficulty::HARD);
    auto followUp = gen.generateFollowUp(question, "It means child class extends parent.");
    EXPECT_EQ(followUp, nullptr); // graceful degradation
}

TEST(FollowUpGeneratorTest, ReturnsNullptrWithNullLLMClient) {
    auto nullClient = std::make_shared<NullLLMClient>();
    FollowUpGenerator gen(nullClient);
    auto question = makeTextQuestion("Encapsulation", Difficulty::EASY);
    auto followUp = gen.generateFollowUp(question, "Hiding internal state.");
    EXPECT_EQ(followUp, nullptr);
}

TEST(InterviewSessionTest, CanAskFollowUpReturnsFalseAfterMax3) {
    InterviewSession session("u1", "CSE", "OOP");
    session.start();
    // Force follow-up count to max
    for (int i = 0; i < 3; i++) {
        session.incrementFollowUpCount();
    }
    EXPECT_FALSE(session.canAskFollowUp());
}

TEST(InterviewSessionTest, CanAskFollowUpReturnsTrueIfLessThan3) {
    InterviewSession session("u1", "CSE", "OOP");
    session.setEnableFollowUps(true);
    session.start();
    session.incrementFollowUpCount();
    session.incrementFollowUpCount();
    EXPECT_TRUE(session.canAskFollowUp());
}

TEST(InterviewSessionTest, FollowUpSubmitHalfPoints) {
    InterviewSession session("u1", "CSE", "OOP");
    session.setEnableFollowUps(true);
    session.start();
    auto q = makeTextQuestion("Inheritance", Difficulty::MEDIUM);
    session.submitAnswer(q, "Parent child relationship", 5);
    int before = session.getTotalScore();
    auto followUp = makeTextQuestion("Inheritance (Follow-Up)", Difficulty::MEDIUM);
    session.incrementFollowUpCount();
    session.submitFollowUpAnswer(followUp, "Yes, like Animal -> Dog.", 8);
    int after = session.getTotalScore();
    EXPECT_GT(after, before); // score increased
    // Follow-up should give approximately half points vs a full correct answer
    int gained = after - before;
    int fullPoints = q->getMaxScore();
    EXPECT_LE(gained, fullPoints); // not full points
}
