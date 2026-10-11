#pragma once

#include "db/IAttemptRepository.h"
#include <mutex>

// OOP Concept: Interface Implementation & Polymorphism
// Flat JSON file repository for user question attempts (data/attempts.json).
class JsonAttemptRepository : public IAttemptRepository {
private:
    std::string filePath_;
    mutable std::mutex mutex_;

    void ensureFileExists() const;

public:
    explicit JsonAttemptRepository(std::string filePath = "data/attempts.json");
    ~JsonAttemptRepository() override = default;

    void save(const QuestionAttempt& attempt) override;
    std::vector<QuestionAttempt> findByUserId(const std::string& userId) override;
    std::vector<QuestionAttempt> findByQuestionId(const std::string& questionId) override;
    std::vector<QuestionAttempt> findByUserIdAndDate(const std::string& userId, const std::string& date) override;
    std::vector<QuestionAttempt> getAll() override;

    const std::string& getFilePath() const noexcept { return filePath_; }
};
