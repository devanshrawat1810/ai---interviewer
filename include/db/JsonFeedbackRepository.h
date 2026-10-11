#pragma once

#include "db/IFeedbackRepository.h"
#include <mutex>

// OOP Concept: Interface Implementation & Polymorphism
// Flat JSON repository storing session feedback reports in data/feedback.json.
class JsonFeedbackRepository : public IFeedbackRepository {
private:
    std::string filePath_;
    mutable std::mutex mutex_;

    void ensureFileExists() const;

public:
    explicit JsonFeedbackRepository(std::string filePath = "data/feedback.json");
    ~JsonFeedbackRepository() override = default;

    void save(const FeedbackReport& feedback) override;
    std::shared_ptr<FeedbackReport> findBySessionId(const std::string& sessionId) override;
    std::vector<FeedbackReport> findByUserId(const std::string& userId) override;
    std::vector<FeedbackReport> getAll() override;
};
