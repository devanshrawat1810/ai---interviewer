#pragma once

#include "db/IAttemptRepository.h"

// OOP Concept: Interface Implementation & Polymorphism
// MongoDB attempt repository satisfying IAttemptRepository.
// Stores and queries student question attempts in the 'attempts' collection.
class MongoAttemptRepository : public IAttemptRepository {
public:
    MongoAttemptRepository() = default;
    ~MongoAttemptRepository() override = default;

    void save(const QuestionAttempt& attempt) override;
    std::vector<QuestionAttempt> findByUserId(const std::string& userId) override;
    std::vector<QuestionAttempt> findByQuestionId(const std::string& questionId) override;
    std::vector<QuestionAttempt> findByUserIdAndDate(const std::string& userId, const std::string& date) override;
    std::vector<QuestionAttempt> getAll() override;
};
