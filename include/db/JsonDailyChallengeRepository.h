#pragma once

#include "db/IDailyChallengeRepository.h"
#include <mutex>

// OOP Concept: Interface Implementation & Polymorphism
// Flat JSON file repository for daily challenges stored in data/daily_challenges.json.
class JsonDailyChallengeRepository : public IDailyChallengeRepository {
private:
    std::string filePath_;
    mutable std::mutex mutex_;

    void ensureFileExists() const;

public:
    explicit JsonDailyChallengeRepository(std::string filePath = "data/daily_challenges.json");
    ~JsonDailyChallengeRepository() override = default;

    void save(const DailyChallengeRecord& record) override;
    std::shared_ptr<DailyChallengeRecord> findByDateAndCourse(
        const std::string& date, const std::string& course) override;
    std::vector<std::string> getPast30DaysQuestionIds(
        const std::string& currentDate, const std::string& course) override;
    std::vector<DailyChallengeRecord> getAll() override;
};
