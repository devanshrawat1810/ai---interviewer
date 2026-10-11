#pragma once

#include "db/IInterviewRepository.h"

// OOP Concept: Interface Implementation & Polymorphism
// MongoDB interview session repository satisfying IInterviewRepository.
// Stores interview records in the 'interviews' collection indexed on userId.
class MongoInterviewRepository : public IInterviewRepository {
public:
    MongoInterviewRepository() = default;
    ~MongoInterviewRepository() override = default;

    void save(const StoredInterviewSession& session) override;
    std::shared_ptr<StoredInterviewSession> findById(const std::string& id) override;
    std::vector<StoredInterviewSession> findByUserId(const std::string& userId) override;
    std::vector<StoredInterviewSession> getAll() override;
};
