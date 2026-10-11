#pragma once

#include "interview/QuestionSelectionStrategy.h"

// OOP Concept: Strategy Pattern Implementation
// Fixed progressive difficulty ramp: Questions 1-3 Easy, Questions 4-7 Medium, Questions 8-10 Hard.
class LinearRampStrategy : public QuestionSelectionStrategy {
public:
    LinearRampStrategy() = default;
    ~LinearRampStrategy() override = default;

    void reset() override;
    Difficulty getNextDifficulty(int questionNumber, const std::vector<bool>& previousResults) override;
    std::shared_ptr<Question> selectNextQuestion(
        int questionNumber,
        const std::vector<std::shared_ptr<Question>>& questionPool,
        const std::vector<std::shared_ptr<Question>>& askedQuestions,
        const std::vector<bool>& previousResults) override;
};
