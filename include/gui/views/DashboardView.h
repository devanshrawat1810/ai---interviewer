#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "gui/presenters/DashboardPresenter.h"
#include "gui/widgets/Card.h"
#include "gui/widgets/PrimaryButton.h"

// OOP Concept: Passive View in Model-View-Presenter
// Candidate dashboard displaying streaks, achievements, daily challenges, and navigation buttons.
class DashboardView : public QWidget {
    Q_OBJECT

public:
    explicit DashboardView(DashboardPresenter* presenter, QWidget* parent = nullptr);

public slots:
    void updateData(const DashboardData& data);

signals:
    void startInterviewClicked();
    void startPracticeClicked();
    void resumeInterviewClicked();
    void dailyChallengeClicked();
    void browseCoursesClicked();
    void leaderboardClicked();
    void profileClicked();
    void adminPanelClicked();
    void logoutClicked();

private:
    DashboardPresenter* presenter_;

    QLabel* greetingLabel_;
    QLabel* collegeLabel_;
    QLabel* noteBannerLabel_;
    QLabel* aiChipLabel_;

    QLabel* streakValLabel_;
    QLabel* bestStreakValLabel_;
    QLabel* scoreValLabel_;
    QLabel* badgesValLabel_;

    Card* dailyCard_;
    QLabel* dailyStatusLabel_;
    QPushButton* dailyStartBtn_;

    QPushButton* adminBtn_;
    void setupUi();
};
