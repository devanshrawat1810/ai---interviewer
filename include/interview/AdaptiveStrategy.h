#pragma once

#include "interview/QuestionSelectionStrategy.h"

// OOP Concept: Strategy Pattern Implementation
// Dynamically adjusts question difficulty according to the candidate's performance streak:
// - 2 consecutive correct answers elevate difficulty (Easy -> Medium -> Hard).
// - 2 consecutive incorrect answers lower difficulty (Hard -> Medium -> Easy).
class AdaptiveStrategy : public QuestionSelectionStrategy {
private:
    Difficulty currentDifficulty_{Difficulty::EASY};
    int consecutiveCorrect_{0};
    int consecutiveWrong_{0};

public:
    explicit AdaptiveStrategy(Difficulty startingDifficulty = Difficulty::EASY);
    ~AdaptiveStrategy() override = default;

    void reset() override;
    Difficulty getNextDifficulty(int questionNumber, const std::vector<bool>& previousResults) override;
    std::shared_ptr<Question> selectNextQuestion(
        int questionNumber,
        const std::vector<std::shared_ptr<Question>>& questionPool,
        const std::vector<std::shared_ptr<Question>>& askedQuestions,
        const std::vector<bool>& previousResults) override;

    // State inspection for testing
    Difficulty getCurrentDifficulty() const noexcept;
    int getConsecutiveCorrect() const noexcept;
    int getConsecutiveWrong() const noexcept;
};
