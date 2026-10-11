#include "interview/AdaptiveStrategy.h"

// OOP Concept: Strategy Pattern Implementation
// Dynamically adjusts difficulty based on consecutive correct/wrong streaks.

AdaptiveStrategy::AdaptiveStrategy(Difficulty startingDifficulty)
    : currentDifficulty_(startingDifficulty),
      consecutiveCorrect_(0),
      consecutiveWrong_(0) {}

void AdaptiveStrategy::reset() {
    currentDifficulty_ = Difficulty::EASY;
    consecutiveCorrect_ = 0;
    consecutiveWrong_ = 0;
}

Difficulty AdaptiveStrategy::getNextDifficulty(int /* questionNumber */,
                                               const std::vector<bool>& previousResults) {
    if (previousResults.empty()) {
        return currentDifficulty_;
    }

    // Recalculate streak from previous results
    int streakCorrect = 0;
    int streakWrong = 0;

    // Check tail of results
    for (auto it = previousResults.rbegin(); it != previousResults.rend(); ++it) {
        if (*it) {
            if (streakWrong > 0) break;
            streakCorrect++;
        } else {
            if (streakCorrect > 0) break;
            streakWrong++;
        }
    }

    consecutiveCorrect_ = streakCorrect;
    consecutiveWrong_ = streakWrong;

    // 2 consecutive correct -> level up
    if (consecutiveCorrect_ >= 2) {
        if (currentDifficulty_ == Difficulty::EASY) {
            currentDifficulty_ = Difficulty::MEDIUM;
            consecutiveCorrect_ = 0;
        } else if (currentDifficulty_ == Difficulty::MEDIUM) {
            currentDifficulty_ = Difficulty::HARD;
            consecutiveCorrect_ = 0;
        }
    }
    // 2 consecutive wrong -> level down
    else if (consecutiveWrong_ >= 2) {
        if (currentDifficulty_ == Difficulty::HARD) {
            currentDifficulty_ = Difficulty::MEDIUM;
            consecutiveWrong_ = 0;
        } else if (currentDifficulty_ == Difficulty::MEDIUM) {
            currentDifficulty_ = Difficulty::EASY;
            consecutiveWrong_ = 0;
        }
    }

    return currentDifficulty_;
}

std::shared_ptr<Question> AdaptiveStrategy::selectNextQuestion(
    int questionNumber,
    const std::vector<std::shared_ptr<Question>>& questionPool,
    const std::vector<std::shared_ptr<Question>>& askedQuestions,
    const std::vector<bool>& previousResults) {
    Difficulty targetDiff = getNextDifficulty(questionNumber, previousResults);

    // Find unasked question of target difficulty
    for (const auto& q : questionPool) {
        if (q->getDifficulty() == targetDiff) {
            bool alreadyAsked = false;
            for (const auto& asked : askedQuestions) {
                if (asked->getId() == q->getId()) {
                    alreadyAsked = true;
                    break;
                }
            }
            if (!alreadyAsked) {
                return q;
            }
        }
    }

    // Fallback: any unasked question
    for (const auto& q : questionPool) {
        bool alreadyAsked = false;
        for (const auto& asked : askedQuestions) {
            if (asked->getId() == q->getId()) {
                alreadyAsked = true;
                break;
            }
        }
        if (!alreadyAsked) {
            return q;
        }
    }

    return nullptr;
}

Difficulty AdaptiveStrategy::getCurrentDifficulty() const noexcept {
    return currentDifficulty_;
}

int AdaptiveStrategy::getConsecutiveCorrect() const noexcept {
    return consecutiveCorrect_;
}

int AdaptiveStrategy::getConsecutiveWrong() const noexcept {
    return consecutiveWrong_;
}
