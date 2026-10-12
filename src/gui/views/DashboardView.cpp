#include "gui/views/DashboardView.h"
#include "gui/ThemeTokens.h"
#include <QScrollArea>

DashboardView::DashboardView(DashboardPresenter* presenter, QWidget* parent)
    : QWidget(parent), presenter_(presenter) {
    setupUi();
    connect(presenter_, &DashboardPresenter::dashboardLoaded, this, &DashboardView::updateData);
}

void DashboardView::setupUi() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(32, 24, 32, 24);
    rootLayout->setSpacing(20);

    // Top Bar (Greeting & AI Chip & Logout)
    auto* topBar = new QHBoxLayout();
    auto* titleCol = new QVBoxLayout();

    greetingLabel_ = new QLabel("Welcome back, Candidate!", this);
    greetingLabel_->setStyleSheet("font-size: 24px; font-weight: bold;");

    collegeLabel_ = new QLabel("Computer Science Track", this);
    collegeLabel_->setStyleSheet("color: #64748B; font-size: 14px;");

    titleCol->addWidget(greetingLabel_);
    titleCol->addWidget(collegeLabel_);

    aiChipLabel_ = new QLabel("AI Features Active", this);
    aiChipLabel_->setStyleSheet(QString("background: #EEF2FF; color: %1; border-radius: 12px; padding: 4px 12px; font-weight: 600; font-size: 12px;")
        .arg(ThemeTokens::ColorPrimary));

    auto* logoutBtn = new QPushButton("Logout", this);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet("background: transparent; color: #DC2626; border: 1px solid #FECACA; border-radius: 6px; padding: 6px 12px;");
    connect(logoutBtn, &QPushButton::clicked, this, &DashboardView::logoutClicked);

    topBar->addLayout(titleCol);
    topBar->addStretch();
    topBar->addWidget(aiChipLabel_);
    topBar->addWidget(logoutBtn);

    rootLayout->addLayout(topBar);

    // Notification Banner
    noteBannerLabel_ = new QLabel(this);
    noteBannerLabel_->setStyleSheet("background: #FEF3C7; color: #92400E; border: 1px solid #FDE68A; border-radius: 8px; padding: 10px; font-weight: 500;");
    noteBannerLabel_->setVisible(false);
    rootLayout->addWidget(noteBannerLabel_);

    // 4 Stats Cards Grid
    auto* statsGrid = new QGridLayout();
    statsGrid->setSpacing(16);

    auto makeStatCard = [](const QString& label, QLabel*& valOut, const QString& icon) {
        auto* card = new Card();
        auto* lay = new QVBoxLayout(card);
        lay->setSpacing(6);
        auto* iconLbl = new QLabel(icon, card);
        iconLbl->setStyleSheet("font-size: 20px;");
        valOut = new QLabel("0", card);
        valOut->setStyleSheet("font-size: 26px; font-weight: bold; color: #1E293B;");
        auto* descLbl = new QLabel(label, card);
        descLbl->setStyleSheet("color: #64748B; font-size: 13px; font-weight: 500;");
        lay->addWidget(iconLbl);
        lay->addWidget(valOut);
        lay->addWidget(descLbl);
        return card;
    };

    statsGrid->addWidget(makeStatCard("Current Streak", streakValLabel_, "🔥"), 0, 0);
    statsGrid->addWidget(makeStatCard("Best Streak", bestStreakValLabel_, "⭐"), 0, 1);
    statsGrid->addWidget(makeStatCard("Total Score", scoreValLabel_, "🎯"), 0, 2);
    statsGrid->addWidget(makeStatCard("Badges Earned", badgesValLabel_, "🏆"), 0, 3);

    rootLayout->addLayout(statsGrid);

    // Daily Challenge Hero Card
    dailyCard_ = new Card(this);
    auto* dailyLayout = new QHBoxLayout(dailyCard_);
    auto* dailyCol = new QVBoxLayout();

    auto* dailyTitle = new QLabel("Today's Daily Challenge (+50 Bonus Points)", dailyCard_);
    dailyTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #0F172A;");
    dailyStatusLabel_ = new QLabel("Solve the daily question to maintain your practice streak!", dailyCard_);
    dailyStatusLabel_->setStyleSheet("color: #64748B; font-size: 13px;");

    dailyCol->addWidget(dailyTitle);
    dailyCol->addWidget(dailyStatusLabel_);

    dailyStartBtn_ = new PrimaryButton("Start Daily Challenge", dailyCard_);
    dailyStartBtn_->setFixedWidth(180);
    connect(dailyStartBtn_, &QPushButton::clicked, this, &DashboardView::dailyChallengeClicked);

    dailyLayout->addLayout(dailyCol);
    dailyLayout->addStretch();
    dailyLayout->addWidget(dailyStartBtn_);

    rootLayout->addWidget(dailyCard_);

    // Action Launchers Grid
    auto* actionsGrid = new QGridLayout();
    actionsGrid->setSpacing(14);

    auto makeActionBtn = [this](const QString& title, const QString& sub, auto signal) {
        auto* btn = new QPushButton(this);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setMinimumHeight(70);
        btn->setStyleSheet(R"(
            QPushButton {
                background-color: #FFFFFF;
                border: 1px solid #E2E8F0;
                border-radius: 10px;
                text-align: left;
                padding: 12px 16px;
            }
            QPushButton:hover {
                border-color: #4F46E5;
                background-color: #F8FAFC;
            }
        )");
        auto* l = new QVBoxLayout(btn);
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(2);
        auto* t = new QLabel(title, btn);
        t->setStyleSheet("font-weight: bold; font-size: 14px; color: #1E293B;");
        auto* s = new QLabel(sub, btn);
        s->setStyleSheet("font-size: 12px; color: #64748B;");
        l->addWidget(t);
        l->addWidget(s);
        connect(btn, &QPushButton::clicked, this, signal);
        return btn;
    };

    actionsGrid->addWidget(makeActionBtn("🚀 Full Mock Interview", "10 questions with progressive ramp (Easy -> Hard)", &DashboardView::startInterviewClicked), 0, 0);
    actionsGrid->addWidget(makeActionBtn("⚡ Adaptive Practice Session", "5 questions adapting dynamically to your answers", &DashboardView::startPracticeClicked), 0, 1);
    actionsGrid->addWidget(makeActionBtn("📄 Resume-Based Interview", "Tailored questions drawn from your uploaded resume", &DashboardView::resumeInterviewClicked), 1, 0);
    actionsGrid->addWidget(makeActionBtn("📊 Campus Leaderboard", "Compare rank globally, by college, or by branch", &DashboardView::leaderboardClicked), 1, 1);
    actionsGrid->addWidget(makeActionBtn("👤 Student Profile & Calendar", "View streak calendar, statistics, and unlocked badges", &DashboardView::profileClicked), 2, 0);

    adminBtn_ = makeActionBtn("⚙️ Admin Governance Panel", "Manage curriculum questions and view system telemetry", &DashboardView::adminPanelClicked);
    adminBtn_->setVisible(false);
    actionsGrid->addWidget(adminBtn_, 2, 1);

    rootLayout->addLayout(actionsGrid);
    rootLayout->addStretch();
}

void DashboardView::updateData(const DashboardData& data) {
    greetingLabel_->setText(QString("Welcome back, %1!").arg(data.candidateName));
    collegeLabel_->setText(data.collegeBranch);

    streakValLabel_->setText(QString("%1d").arg(data.streak));
    bestStreakValLabel_->setText(QString("%1d").arg(data.longestStreak));
    scoreValLabel_->setText(QString("%1 pts").arg(data.totalScore));
    badgesValLabel_->setText(QString::number(data.badges.size()));

    if (!data.notificationBanner.trimmed().isEmpty()) {
        noteBannerLabel_->setText(data.notificationBanner);
        noteBannerLabel_->setVisible(true);
    } else {
        noteBannerLabel_->setVisible(false);
    }

    if (data.aiConfigured) {
        aiChipLabel_->setText(QString("AI Features: %1 (%2 calls left today)").arg(data.aiProvider).arg(data.dailyAiRemaining));
        aiChipLabel_->setStyleSheet(QString("background: #EEF2FF; color: %1; border-radius: 12px; padding: 4px 12px; font-weight: 600; font-size: 12px;")
            .arg(ThemeTokens::ColorPrimary));
    } else {
        aiChipLabel_->setText("AI Features: Offline (Rule-based)");
        aiChipLabel_->setStyleSheet("background: #F1F5F9; color: #64748B; border-radius: 12px; padding: 4px 12px; font-weight: 600; font-size: 12px;");
    }

    if (data.isDailyDone) {
        dailyStatusLabel_->setText("✅ You have already completed today's challenge! Streak secured.");
        dailyStartBtn_->setEnabled(false);
        dailyStartBtn_->setText("Completed");
    } else {
        dailyStatusLabel_->setText("Challenge ready! Solve it now to advance your streak.");
        dailyStartBtn_->setEnabled(true);
        dailyStartBtn_->setText("Start Daily Challenge");
    }

    adminBtn_->setVisible(data.isAdmin);
}
