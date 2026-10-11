#pragma once

#include <vector>
#include <memory>
#include "core/Question.h"
#include "core/Difficulty.h"

// OOP Concept: Strategy Pattern
// Abstract strategy decoupling the question sequencing algorithm from the interview runner.
class QuestionSelectionStrategy {
public:
    virtual ~QuestionSelectionStrategy() = default;

    // Resets internal state before starting a new session
    virtual void reset() = 0;

    // Determines the difficulty for the next question given current question number (1-based)
    // and history of previous answer results.
    virtual Difficulty getNextDifficulty(int questionNumber, const std::vector<bool>& previousResults) = 0;

    // Selects a question from available pool matching the target difficulty
    virtual std::shared_ptr<Question> selectNextQuestion(
        int questionNumber,
        const std::vector<std::shared_ptr<Question>>& questionPool,
        const std::vector<std::shared_ptr<Question>>& askedQuestions,
        const std::vector<bool>& previousResults) = 0;
};
