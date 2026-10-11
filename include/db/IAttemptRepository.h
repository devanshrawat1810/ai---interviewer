#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/Difficulty.h"

// Record of a question attempt
struct QuestionAttempt {
    std::string id;
    std::string userId;
    std::string questionId;
    std::string subject;
    Difficulty difficulty;
    std::string userResponse;
    bool isCorrect{false};
    int timeTakenSeconds{0};
    int pointsEarned{0};
    std::string date; // ISO 8601 or YYYY-MM-DD
};

// OOP Concept: Dependency Inversion Principle
// Abstract repository interface for individual question attempts.
class IAttemptRepository {
public:
    virtual ~IAttemptRepository() = default;

    virtual void save(const QuestionAttempt& attempt) = 0;
    virtual std::vector<QuestionAttempt> findByUserId(const std::string& userId) = 0;
    virtual std::vector<QuestionAttempt> findByQuestionId(const std::string& questionId) = 0;
    virtual std::vector<QuestionAttempt> findByUserIdAndDate(const std::string& userId, const std::string& date) = 0;
    virtual std::vector<QuestionAttempt> getAll() = 0;
};
