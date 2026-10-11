#pragma once

#include "db/IQuestionRepository.h"
#include <mutex>

// OOP Concept: Interface Implementation & Polymorphism
// JSON directory-backed question repository. Loads and indexes questions from data/questions/*.json.
class JsonQuestionRepository : public IQuestionRepository {
private:
    std::string directoryPath_;
    mutable std::mutex mutex_;
    mutable std::vector<std::shared_ptr<Question>> cache_;
    mutable bool isLoaded_{false};

    void loadAllQuestions() const;

public:
    explicit JsonQuestionRepository(std::string directoryPath = "data/questions");
    ~JsonQuestionRepository() override = default;

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

    void reload();
};
