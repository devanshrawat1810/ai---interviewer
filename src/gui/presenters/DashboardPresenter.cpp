#include "gui/presenters/DashboardPresenter.h"
#include "gui/ServiceContext.h"
#include "core/Student.h"
#include "core/User.h"
#include <QtConcurrent>

DashboardPresenter::DashboardPresenter(std::shared_ptr<ServiceContext> ctx, QObject* parent)
    : QObject(parent), ctx_(std::move(ctx)) {}

void DashboardPresenter::loadDashboard(std::shared_ptr<User> user) {
    if (!user) return;

    (void)QtConcurrent::run([this, user]() {
        DashboardData data;
        data.candidateName = QString::fromStdString(user->getName());
        data.isAdmin = user->isAdmin();

        if (auto s = std::dynamic_pointer_cast<Student>(user)) {
            data.collegeBranch = QString("%1 (%2)")
                .arg(QString::fromStdString(s->getCollege()))
                .arg(QString::fromStdString(s->getBranch()));
            data.courseTrack = QString::fromStdString(s->getCourse());
            data.streak = s->getStreak();
            data.longestStreak = s->getLongestStreak();
            data.totalScore = s->getTotalScore();
            data.badges = s->getBadges();

            // Daily challenge status
            auto dailyQ = ctx_->dailyService->getDailyQuestion(s->getCourse());
            data.hasDailyChallenge = (dailyQ != nullptr);
            data.isDailyDone = (s->getLastActiveDate() == ctx_->dailyService->getCurrentDate());

            // Notification Banner
            auto note = ctx_->noteService->checkNotifications(*s, s->getCourse());
            data.notificationBanner = QString::fromStdString(ctx_->noteService->renderBanner(note));

            // AI Status & Quota
            data.aiConfigured = ctx_->aiFeedback->isConfigured();
            data.aiProvider = QString::fromStdString(ctx_->aiFeedback->getProviderName());
            data.dailyAiRemaining = data.aiConfigured ? ctx_->usageTracker->getRemainingCalls(s->getId()) : 0;
        }

        QMetaObject::invokeMethod(this, [this, data]() {
            emit dashboardLoaded(data);
        }, Qt::QueuedConnection);
    });
}
