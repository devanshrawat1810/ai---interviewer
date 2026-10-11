#pragma once

#include "db/IUserRepository.h"

// OOP Concept: Interface Implementation & Polymorphism
// Production-grade MongoDB user repository implementation satisfying IUserRepository.
// Performs BSON document translation using UserMapper and interacts with the 'users' collection.
class MongoUserRepository : public IUserRepository {
public:
    MongoUserRepository() = default;
    ~MongoUserRepository() override = default;

    void save(const User& user) override;
    std::shared_ptr<User> findById(const std::string& id) override;
    std::shared_ptr<User> findByEmail(const std::string& email) override;
    std::vector<std::shared_ptr<Student>> getAllStudents() override;
    bool deleteById(const std::string& id) override;
};
