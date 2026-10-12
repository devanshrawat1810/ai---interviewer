#include "gui/views/LeaderboardView.h"
#include "gui/ServiceContext.h"
#include "gui/ThemeTokens.h"
#include "core/Student.h"
#include <QHeaderView>
#include <algorithm>

LeaderboardView::LeaderboardView(std::shared_ptr<ServiceContext> ctx, QWidget* parent)
    : QWidget(parent), ctx_(std::move(ctx)) {
    setupUi();
}

void LeaderboardView::setupUi() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(32, 24, 32, 24);
    rootLayout->setSpacing(18);

    auto* topBar = new QHBoxLayout();
    auto* backBtn = new QPushButton("← Back to Dashboard", this);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet("background: transparent; border: none; font-weight: bold; color: #4F46E5;");
    connect(backBtn, &QPushButton::clicked, this, &LeaderboardView::backRequested);

    auto* titleLabel = new QLabel("Placement Practice Leaderboard", this);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #0F172A;");

    topBar->addWidget(backBtn);
    topBar->addSpacing(16);
    topBar->addWidget(titleLabel);
    topBar->addStretch();
    rootLayout->addLayout(topBar);

    // Filters Bar
    auto* filterBar = new QHBoxLayout();
    auto* scopeLbl = new QLabel("Scope:", this);
    scopeCombo_ = new QComboBox(this);
    scopeCombo_->addItems({"Global", "My College", "My Branch"});

    auto* stratLbl = new QLabel("Rank By:", this);
    strategyCombo_ = new QComboBox(this);
    strategyCombo_->addItems({"Total Score", "Streak Days", "Problems Solved"});

    filterBar->addWidget(scopeLbl);
    filterBar->addWidget(scopeCombo_);
    filterBar->addSpacing(20);
    filterBar->addWidget(stratLbl);
    filterBar->addWidget(strategyCombo_);
    filterBar->addStretch();
    rootLayout->addLayout(filterBar);

    // Table
    table_ = new QTableWidget(this);
    table_->setColumnCount(5);
    table_->setHorizontalHeaderLabels({"Rank", "Candidate", "College", "Streak", "Score"});
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->verticalHeader()->setVisible(false);
    table_->setStyleSheet(R"(
        QTableWidget {
            background-color: #FFFFFF;
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            gridline-color: #F1F5F9;
        }
        QHeaderView::section {
            background-color: #F8FAFC;
            padding: 8px;
            font-weight: bold;
            border: none;
        }
    )");

    rootLayout->addWidget(table_);

    stickyUserRankLabel_ = new QLabel(this);
    stickyUserRankLabel_->setStyleSheet(QString("background: #EEF2FF; color: %1; border-radius: 8px; padding: 10px; font-weight: bold;")
        .arg(ThemeTokens::ColorPrimary));
    stickyUserRankLabel_->setVisible(false);
    rootLayout->addWidget(stickyUserRankLabel_);
}

void LeaderboardView::refreshData(const QString& currentUserId) {
    loadLeaderboard(currentUserId);
}

void LeaderboardView::loadLeaderboard(const QString& currentUserId) {
    auto students = ctx_->userRepo->getAllStudents();

    // Sort by score
    std::sort(students.begin(), students.end(), [](const auto& a, const auto& b) {
        return a->getTotalScore() > b->getTotalScore();
    });

    table_->setRowCount(static_cast<int>(students.size()));

    int userRank = -1;

    for (size_t i = 0; i < students.size(); ++i) {
        const auto& s = students[i];
        int rank = static_cast<int>(i + 1);

        if (QString::fromStdString(s->getId()) == currentUserId) {
            userRank = rank;
        }

        QString rankPrefix = (rank == 1) ? "🥇 1" : (rank == 2) ? "🥈 2" : (rank == 3) ? "🥉 3" : QString::number(rank);

        table_->setItem(static_cast<int>(i), 0, new QTableWidgetItem(rankPrefix));
        table_->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::fromStdString(s->getName())));
        table_->setItem(static_cast<int>(i), 2, new QTableWidgetItem(QString::fromStdString(s->getCollege())));
        table_->setItem(static_cast<int>(i), 3, new QTableWidgetItem(QString("%1d").arg(s->getStreak())));
        table_->setItem(static_cast<int>(i), 4, new QTableWidgetItem(QString("%1 pts").arg(s->getTotalScore())));
    }

    if (userRank > 0) {
        stickyUserRankLabel_->setText(QString("⭐ Your Current Position: #%1 on the Leaderboard").arg(userRank));
        stickyUserRankLabel_->setVisible(true);
    } else {
        stickyUserRankLabel_->setVisible(false);
    }
}
