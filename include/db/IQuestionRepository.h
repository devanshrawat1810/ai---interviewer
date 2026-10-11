#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/Question.h"
#include "core/Difficulty.h"

// OOP Concept: Dependency Inversion Principle
// Abstract repository interface for question management and querying across curriculum dimensions.
class IQuestionRepository {
public:
    virtual ~IQuestionRepository() = default;

    virtual void save(const Question& question) = 0;
    virtual std::shared_ptr<Question> findById(const std::string& id) = 0;
    virtual std::vector<std::shared_ptr<Question>> findBySubject(const std::string& subject) = 0;
    virtual std::vector<std::shared_ptr<Question>> findByDifficulty(Difficulty difficulty) = 0;
    virtual std::vector<std::shared_ptr<Question>> findBySubjectAndDifficulty(
        const std::string& subject, Difficulty difficulty) = 0;
    virtual std::vector<std::shared_ptr<Question>> findByFilters(
        const std::string& course,
        const std::string& subject,
        const std::string& topic,
        Difficulty difficulty) = 0;
    virtual std::vector<std::shared_ptr<Question>> getAll() = 0;
};
