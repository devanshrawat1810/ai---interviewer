#pragma once

#include "db/IInterviewRepository.h"
#include <mutex>

// OOP Concept: Interface Implementation & Polymorphism
// Flat JSON repository storing interview sessions in data/interviews.json.
class JsonInterviewRepository : public IInterviewRepository {
private:
    std::string filePath_;
    mutable std::mutex mutex_;

    void ensureFileExists() const;

public:
    explicit JsonInterviewRepository(std::string filePath = "data/interviews.json");
    ~JsonInterviewRepository() override = default;

    void save(const StoredInterviewSession& session) override;
    std::shared_ptr<StoredInterviewSession> findById(const std::string& id) override;
    std::vector<StoredInterviewSession> findByUserId(const std::string& userId) override;
    std::vector<StoredInterviewSession> getAll() override;
};
