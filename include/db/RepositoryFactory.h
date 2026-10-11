#pragma once

#include <memory>
#include <string>
#include "db/IUserRepository.h"
#include "db/IQuestionRepository.h"
#include "db/IAttemptRepository.h"
#include "db/IInterviewRepository.h"
#include "db/IFeedbackRepository.h"
#include "db/IScoreboardRepository.h"
#include "db/IDailyChallengeRepository.h"
#include "db/IQuestionFeedbackRepository.h"
#include "db/IAiUsageRepository.h"

// Aggregated container of repository dependencies injected across the system
struct RepositoryBundle {
    std::unique_ptr<IUserRepository> userRepo;
    std::unique_ptr<IQuestionRepository> questionRepo;
    std::unique_ptr<IAttemptRepository> attemptRepo;
    std::unique_ptr<IInterviewRepository> interviewRepo;
    std::unique_ptr<IFeedbackRepository> feedbackRepo;
    std::unique_ptr<IScoreboardRepository> scoreboardRepo;
    std::unique_ptr<IDailyChallengeRepository> dailyRepo;
    std::unique_ptr<IQuestionFeedbackRepository> questionFeedbackRepo;
    std::unique_ptr<IAiUsageRepository> aiUsageRepo;
    std::string activeBackend; // "mongo" or "json"
};

// OOP Concept: Factory Method & Dependency Inversion Principle
// Encapsulates repository instantiation logic based on STORAGE_BACKEND environment variable.
// Gracefully falls back to JSON file repositories if MongoDB is selected but unreachable.
class RepositoryFactory {
public:
    static RepositoryBundle createRepositories(const std::string& preferredBackend = "");

    static std::unique_ptr<IUserRepository> createUserRepository(const std::string& backend = "");
    static std::unique_ptr<IQuestionRepository> createQuestionRepository(const std::string& backend = "");
    static std::unique_ptr<IAttemptRepository> createAttemptRepository(const std::string& backend = "");
    static std::unique_ptr<IInterviewRepository> createInterviewRepository(const std::string& backend = "");
    static std::unique_ptr<IFeedbackRepository> createFeedbackRepository(const std::string& backend = "");
    static std::unique_ptr<IScoreboardRepository> createScoreboardRepository(const std::string& backend = "");
    static std::unique_ptr<IDailyChallengeRepository> createDailyChallengeRepository(const std::string& backend = "");
    static std::unique_ptr<IQuestionFeedbackRepository> createQuestionFeedbackRepository(const std::string& backend = "");
    static std::unique_ptr<IAiUsageRepository> createAiUsageRepository(const std::string& backend = "");

    static std::string resolveBackend(const std::string& preferred);
};
