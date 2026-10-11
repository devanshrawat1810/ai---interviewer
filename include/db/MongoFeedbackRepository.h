#pragma once

#include "db/IFeedbackRepository.h"

// OOP Concept: Interface Implementation & Polymorphism
// MongoDB feedback report repository satisfying IFeedbackRepository.
// Stores generated diagnostic evaluations in the 'feedback' collection indexed on userId.
class MongoFeedbackRepository : public IFeedbackRepository {
public:
    MongoFeedbackRepository() = default;
    ~MongoFeedbackRepository() override = default;

    void save(const FeedbackReport& feedback) override;
    std::shared_ptr<FeedbackReport> findBySessionId(const std::string& sessionId) override;
    std::vector<FeedbackReport> findByUserId(const std::string& userId) override;
    std::vector<FeedbackReport> getAll() override;
};
