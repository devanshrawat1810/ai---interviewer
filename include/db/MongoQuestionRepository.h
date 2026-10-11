#pragma once

#include "db/IQuestionRepository.h"

// OOP Concept: Interface Implementation & Polymorphism
// MongoDB question repository satisfying IQuestionRepository.
// Leverages compound indexes across (course, subject, difficulty) for high-performance querying.
class MongoQuestionRepository : public IQuestionRepository {
public:
    MongoQuestionRepository() = default;
    ~MongoQuestionRepository() override = default;

    void save(const Question& question) override;
    std::shared_ptr<Question> findById(const std::string& id) override;
    std::vector<std::shared_ptr<Question>> findBySubject(const std::string& subject) override;
    std::vector<std::shared_ptr<Question>> findByDifficulty(Difficulty difficulty) override;
    std::vector<std::shared_ptr<Question>> findBySubjectAndDifficulty(
        const std::string& subject, Difficulty difficulty) override;
    std::vector<std::shared_ptr<Question>> findByFilters(
        const std::string& course,
        const std::string& subject,
        const std::string& topic,
        Difficulty difficulty) override;
    std::vector<std::shared_ptr<Question>> getAll() override;
};
