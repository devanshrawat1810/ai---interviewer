#pragma once

#include <string>
#include <vector>
#include <memory>

struct StoredInterviewSession {
    std::string id;
    std::string userId;
    std::string subjectId;
    std::string mode;
    int totalScore{0};
    int maxScore{0};
    int questionsAttempted{0};
    int correctCount{0};
    std::string date; // YYYY-MM-DD HH:MM:SS
};

// OOP Concept: Dependency Inversion Principle
// Interface for storing and retrieving completed interview sessions.
class IInterviewRepository {
public:
    virtual ~IInterviewRepository() = default;

    virtual void save(const StoredInterviewSession& session) = 0;
    virtual std::shared_ptr<StoredInterviewSession> findById(const std::string& id) = 0;
    virtual std::vector<StoredInterviewSession> findByUserId(const std::string& userId) = 0;
    virtual std::vector<StoredInterviewSession> getAll() = 0;
};
