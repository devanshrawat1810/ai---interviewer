#pragma once

#include <QObject>
#include <memory>
#include <QString>
#include <vector>

class ServiceContext;
class Student;
class User;

struct DashboardData {
    QString candidateName;
    QString collegeBranch;
    QString courseTrack;
    int streak{0};
    int longestStreak{0};
    int totalScore{0};
    bool aiConfigured{false};
    QString aiProvider;
    int dailyAiRemaining{0};
    bool hasDailyChallenge{false};
    bool isDailyDone{false};
    QString notificationBanner;
    std::vector<std::string> badges;
    bool isAdmin{false};
};

// OOP Concept: Presenter in Model-View-Presenter
// Computes and serves aggregated candidate telemetry, streak notifications, and challenge statuses.
class DashboardPresenter : public QObject {
    Q_OBJECT

public:
    explicit DashboardPresenter(std::shared_ptr<ServiceContext> ctx, QObject* parent = nullptr);

    void loadDashboard(std::shared_ptr<User> user);

signals:
    void dashboardLoaded(const DashboardData& data);

private:
    std::shared_ptr<ServiceContext> ctx_;
};
