#pragma once

#include "db/IUserRepository.h"
#include <mutex>

// OOP Concept: Interface Implementation & Polymorphism
// Flat JSON file-backed user repository implementing IUserRepository.
// Used as default lightweight storage and seamless offline fallback.
class JsonUserRepository : public IUserRepository {
private:
    std::string filePath_;
    mutable std::mutex mutex_;

    void ensureFileExists() const;

public:
    explicit JsonUserRepository(std::string filePath = "data/users.json");
    ~JsonUserRepository() override = default;

    void save(const User& user) override;
    std::shared_ptr<User> findById(const std::string& id) override;
    std::shared_ptr<User> findByEmail(const std::string& email) override;
    std::vector<std::shared_ptr<Student>> getAllStudents() override;
    bool deleteById(const std::string& id) override;

    const std::string& getFilePath() const noexcept { return filePath_; }
};
