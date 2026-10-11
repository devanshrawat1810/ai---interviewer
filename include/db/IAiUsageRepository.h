#pragma once

#include <string>

// OOP Concept: Interface & Dependency Inversion Principle
// Abstract persistence contract for tracking candidate AI token consumption and daily call limits.
class IAiUsageRepository {
public:
    virtual ~IAiUsageRepository() = default;

    virtual int getUsage(const std::string& userId, const std::string& date) = 0;
    virtual void incrementUsage(const std::string& userId, const std::string& date, int count = 1) = 0;

    virtual int getReviewUsage(const std::string& userId, const std::string& date) = 0;
    virtual void incrementReviewUsage(const std::string& userId, const std::string& date) = 0;
};
