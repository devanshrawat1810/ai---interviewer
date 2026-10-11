#pragma once

#include <string>
#include <vector>
#include <memory>
#include "feedback/FeedbackGenerator.h"

// OOP Concept: Dependency Inversion Principle
// Interface for storing and querying generated feedback reports.
class IFeedbackRepository {
public:
    virtual ~IFeedbackRepository() = default;

    virtual void save(const FeedbackReport& feedback) = 0;
    virtual std::shared_ptr<FeedbackReport> findBySessionId(const std::string& sessionId) = 0;
    virtual std::vector<FeedbackReport> findByUserId(const std::string& userId) = 0;
    virtual std::vector<FeedbackReport> getAll() = 0;
};
