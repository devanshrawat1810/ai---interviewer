#include "gui/ServiceContext.h"
#include "core/SystemClock.h"
#include "ai/LLMClientFactory.h"

std::shared_ptr<ServiceContext> ServiceContext::createDefault() {
    auto ctx = std::make_shared<ServiceContext>();

    // Repositories via existing factory
    auto bundle = RepositoryFactory::createRepositories();
    ctx->userRepo = std::move(bundle.userRepo);
    ctx->questionRepo = std::move(bundle.questionRepo);
    ctx->attemptRepo = std::move(bundle.attemptRepo);
    ctx->dailyRepo = std::move(bundle.dailyRepo);
    ctx->feedbackRepo = std::move(bundle.questionFeedbackRepo);
    ctx->aiUsageRepo = std::move(bundle.aiUsageRepo);

    ctx->clock = std::make_shared<SystemClock>();

    // Business Services
    ctx->authService = std::make_shared<AuthService>(*ctx->userRepo);
    ctx->adminService = std::make_shared<AdminService>(
        *ctx->userRepo, *ctx->questionRepo, *ctx->attemptRepo, *ctx->feedbackRepo);
    ctx->dailyService = std::make_shared<DailyChallengeService>(
        *ctx->dailyRepo, *ctx->questionRepo, *ctx->clock);
    ctx->streakService = std::make_shared<StreakService>(*ctx->userRepo, *ctx->clock);
    ctx->reviewService = std::make_shared<ReviewService>(
        *ctx->attemptRepo, *ctx->questionRepo, *ctx->userRepo, *ctx->feedbackRepo);
    ctx->noteService = std::make_shared<NotificationService>(*ctx->streakService, *ctx->dailyService);

    // AI Services via LLMClientFactory
    auto llmClient = LLMClientFactory::createClientFromEnv();
    ctx->aiFeedback = std::make_shared<AIFeedback>(llmClient);
    ctx->resumeAnalyzer = std::make_shared<ResumeAnalyzer>(llmClient);
    ctx->usageTracker = std::make_shared<UsageTracker>(ctx->aiUsageRepo, ctx->clock);
    ctx->answerReviewer = std::make_shared<AnswerReviewer>(llmClient, nullptr, ctx->usageTracker);

    return ctx;
}
