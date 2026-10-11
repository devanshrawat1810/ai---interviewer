#pragma once

#include "db/IDailyChallengeRepository.h"

// OOP Concept: Interface Implementation & Polymorphism
// MongoDB storage implementation for daily challenge schedules.
class MongoDailyChallengeRepository : public IDailyChallengeRepository {
public:
    MongoDailyChallengeRepository() = default;
    ~MongoDailyChallengeRepository() override = default;

    void save(const DailyChallengeRecord& record) override;
    std::shared_ptr<DailyChallengeRecord> findByDateAndCourse(
        const std::string& date, const std::string& course) override;
    std::vector<std::string> getPast30DaysQuestionIds(
        const std::string& currentDate, const std::string& course) override;
    std::vector<DailyChallengeRecord> getAll() override;
};
