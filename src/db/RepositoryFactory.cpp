#include "db/RepositoryFactory.h"
#include "db/Database.h"
#include "db/JsonUserRepository.h"
#include "db/JsonQuestionRepository.h"
#include "db/JsonAttemptRepository.h"
#include "db/JsonInterviewRepository.h"
#include "db/JsonFeedbackRepository.h"
#include "db/JsonScoreboardRepository.h"
#include "db/JsonDailyChallengeRepository.h"
#include "db/JsonQuestionFeedbackRepository.h"
#include "db/MongoUserRepository.h"
#include "db/MongoQuestionRepository.h"
#include "db/MongoAttemptRepository.h"
#include "db/MongoInterviewRepository.h"
#include "db/MongoFeedbackRepository.h"
#include "db/MongoScoreboardRepository.h"
#include "db/MongoDailyChallengeRepository.h"
#include "db/MongoQuestionFeedbackRepository.h"
#include "db/JsonAiUsageRepository.h"
#include "db/MongoAiUsageRepository.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

// OOP Concept: Factory Method Pattern Implementation
// Dynamically provisions repository suites based on configuration and server liveness.

std::string RepositoryFactory::resolveBackend(const std::string& preferred) {
    std::string backend = preferred;
    if (backend.empty()) {
        const char* envVal = std::getenv("STORAGE_BACKEND");
        if (envVal != nullptr && *envVal != '\0') {
            backend = envVal;
        } else {
            backend = "json";
        }
    }

    std::transform(backend.begin(), backend.end(), backend.begin(), ::tolower);

    if (backend == "mongo" || backend == "mongodb") {
        bool connected = Database::getInstance().connect();
        if (!connected || !Database::getInstance().ping()) {
            std::cerr << "\n[RepositoryFactory] WARNING: STORAGE_BACKEND=mongo requested, "
                      << "but MongoDB at '" << Database::getInstance().getUri()
                      << "' is unreachable.\n"
                      << "[RepositoryFactory] Automatically falling back to local JSON file repository backend.\n\n";
            return "json";
        }
        return "mongo";
    }

    return "json";
}

RepositoryBundle RepositoryFactory::createRepositories(const std::string& preferredBackend) {
    std::string active = resolveBackend(preferredBackend);
    RepositoryBundle bundle;
    bundle.activeBackend = active;

    if (active == "mongo") {
        bundle.userRepo = std::make_unique<MongoUserRepository>();
        bundle.questionRepo = std::make_unique<MongoQuestionRepository>();
        bundle.attemptRepo = std::make_unique<MongoAttemptRepository>();
        bundle.interviewRepo = std::make_unique<MongoInterviewRepository>();
        bundle.feedbackRepo = std::make_unique<MongoFeedbackRepository>();
        bundle.scoreboardRepo = std::make_unique<MongoScoreboardRepository>();
        bundle.dailyRepo = std::make_unique<MongoDailyChallengeRepository>();
        bundle.questionFeedbackRepo = std::make_unique<MongoQuestionFeedbackRepository>();
        bundle.aiUsageRepo = std::make_unique<MongoAiUsageRepository>();
    } else {
        bundle.userRepo = std::make_unique<JsonUserRepository>();
        bundle.questionRepo = std::make_unique<JsonQuestionRepository>();
        bundle.attemptRepo = std::make_unique<JsonAttemptRepository>();
        bundle.interviewRepo = std::make_unique<JsonInterviewRepository>();
        bundle.feedbackRepo = std::make_unique<JsonFeedbackRepository>();
        bundle.scoreboardRepo = std::make_unique<JsonScoreboardRepository>();
        bundle.dailyRepo = std::make_unique<JsonDailyChallengeRepository>();
        bundle.questionFeedbackRepo = std::make_unique<JsonQuestionFeedbackRepository>();
        bundle.aiUsageRepo = std::make_unique<JsonAiUsageRepository>();
    }

    return bundle;
}

std::unique_ptr<IUserRepository> RepositoryFactory::createUserRepository(const std::string& backend) {
    std::string active = resolveBackend(backend);
    if (active == "mongo") return std::make_unique<MongoUserRepository>();
    return std::make_unique<JsonUserRepository>();
}

std::unique_ptr<IQuestionRepository> RepositoryFactory::createQuestionRepository(const std::string& backend) {
    std::string active = resolveBackend(backend);
    if (active == "mongo") return std::make_unique<MongoQuestionRepository>();
    return std::make_unique<JsonQuestionRepository>();
}

std::unique_ptr<IAttemptRepository> RepositoryFactory::createAttemptRepository(const std::string& backend) {
    std::string active = resolveBackend(backend);
    if (active == "mongo") return std::make_unique<MongoAttemptRepository>();
    return std::make_unique<JsonAttemptRepository>();
}

std::unique_ptr<IInterviewRepository> RepositoryFactory::createInterviewRepository(const std::string& backend) {
    std::string active = resolveBackend(backend);
    if (active == "mongo") return std::make_unique<MongoInterviewRepository>();
    return std::make_unique<JsonInterviewRepository>();
}

std::unique_ptr<IFeedbackRepository> RepositoryFactory::createFeedbackRepository(const std::string& backend) {
    std::string active = resolveBackend(backend);
    if (active == "mongo") return std::make_unique<MongoFeedbackRepository>();
    return std::make_unique<JsonFeedbackRepository>();
}

std::unique_ptr<IScoreboardRepository> RepositoryFactory::createScoreboardRepository(const std::string& backend) {
    std::string active = resolveBackend(backend);
    if (active == "mongo") return std::make_unique<MongoScoreboardRepository>();
    return std::make_unique<JsonScoreboardRepository>();
}

std::unique_ptr<IDailyChallengeRepository> RepositoryFactory::createDailyChallengeRepository(const std::string& backend) {
    std::string active = resolveBackend(backend);
    if (active == "mongo") return std::make_unique<MongoDailyChallengeRepository>();
    return std::make_unique<JsonDailyChallengeRepository>();
}

std::unique_ptr<IQuestionFeedbackRepository> RepositoryFactory::createQuestionFeedbackRepository(const std::string& backend) {
    std::string active = resolveBackend(backend);
    if (active == "mongo") return std::make_unique<MongoQuestionFeedbackRepository>();
    return std::make_unique<JsonQuestionFeedbackRepository>();
}

std::unique_ptr<IAiUsageRepository> RepositoryFactory::createAiUsageRepository(const std::string& backend) {
    std::string active = resolveBackend(backend);
    if (active == "mongo") return std::make_unique<MongoAiUsageRepository>();
    return std::make_unique<JsonAiUsageRepository>();
}

