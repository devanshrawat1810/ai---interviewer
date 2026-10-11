#include "interview/LinearRampStrategy.h"
#include <algorithm>

// OOP Concept: Strategy Pattern Implementation
// Determines difficulty strictly based on the question progression index:
// 1-3 Easy, 4-7 Medium, 8-10 Hard.

void LinearRampStrategy::reset() {
    // Stateless ramp does not require internal state clearance
}

Difficulty LinearRampStrategy::getNextDifficulty(int questionNumber,
                                                 const std::vector<bool>& /* previousResults */) {
    if (questionNumber <= 3) {
        return Difficulty::EASY;
    } else if (questionNumber <= 7) {
        return Difficulty::MEDIUM;
    } else {
        return Difficulty::HARD;
    }
}

std::shared_ptr<Question> LinearRampStrategy::selectNextQuestion(
    int questionNumber,
    const std::vector<std::shared_ptr<Question>>& questionPool,
    const std::vector<std::shared_ptr<Question>>& askedQuestions,
    const std::vector<bool>& previousResults) {
    Difficulty targetDiff = getNextDifficulty(questionNumber, previousResults);

    // Filter questionPool for target difficulty not yet asked
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
