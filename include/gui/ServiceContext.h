#pragma once

#include "auth/AuthService.h"
#include "admin/AdminService.h"
#include "interview/InterviewSession.h"
#include "interview/DailyChallengeService.h"
#include "interview/StreakService.h"
#include "interview/ReviewService.h"
#include "interview/NotificationService.h"
#include "feedback/AIFeedback.h"
#include "ai/ResumeAnalyzer.h"
#include "ai/AnswerReviewer.h"
#include "ai/UsageTracker.h"
#include "core/IClock.h"
#include "db/RepositoryFactory.h"

#include <memory>

// OOP Concept: Dependency Injection Container
// Holds singleton services instantiated via existing factories to be passed into presenters.
class ServiceContext {
public:
    std::shared_ptr<IUserRepository> userRepo;
    std::shared_ptr<IQuestionRepository> questionRepo;
    std::shared_ptr<IAttemptRepository> attemptRepo;
    std::shared_ptr<IDailyChallengeRepository> dailyRepo;
    std::shared_ptr<IQuestionFeedbackRepository> feedbackRepo;
    std::shared_ptr<IAiUsageRepository> aiUsageRepo;

    std::shared_ptr<IClock> clock;
    std::shared_ptr<AuthService> authService;
    std::shared_ptr<AdminService> adminService;
    std::shared_ptr<DailyChallengeService> dailyService;
    std::shared_ptr<StreakService> streakService;
    std::shared_ptr<ReviewService> reviewService;
    std::shared_ptr<NotificationService> noteService;
    std::shared_ptr<AIFeedback> aiFeedback;
    std::shared_ptr<ResumeAnalyzer> resumeAnalyzer;
    std::shared_ptr<AnswerReviewer> answerReviewer;
    std::shared_ptr<UsageTracker> usageTracker;

    static std::shared_ptr<ServiceContext> createDefault();
};
