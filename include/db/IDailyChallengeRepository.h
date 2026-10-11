#pragma once

#include <string>
#include <vector>
#include <memory>

struct DailyChallengeRecord {
    std::string id;
    std::string date;     // YYYY-MM-DD
    std::string course;   // Course ID / Name (e.g. CSE)
    std::string questionId;
};

// OOP Concept: Dependency Inversion Principle
// Abstract repository for persisting daily challenge schedules and past 30-day question usage.
class IDailyChallengeRepository {
public:
    virtual ~IDailyChallengeRepository() = default;

    virtual void save(const DailyChallengeRecord& record) = 0;
    virtual std::shared_ptr<DailyChallengeRecord> findByDateAndCourse(
        const std::string& date, const std::string& course) = 0;
    virtual std::vector<std::string> getPast30DaysQuestionIds(
        const std::string& currentDate, const std::string& course) = 0;
    virtual std::vector<DailyChallengeRecord> getAll() = 0;
};
