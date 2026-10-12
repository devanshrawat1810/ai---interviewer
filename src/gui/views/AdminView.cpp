#include "gui/views/AdminView.h"
#include "gui/ServiceContext.h"
#include "core/Admin.h"
#include <QHeaderView>
#include <QMessageBox>

AdminView::AdminView(std::shared_ptr<ServiceContext> ctx, QWidget* parent)
    : QWidget(parent), ctx_(std::move(ctx)) {
    setupUi();
}

void AdminView::setupUi() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(32, 24, 32, 24);
    rootLayout->setSpacing(18);

    auto* topBar = new QHBoxLayout();
    auto* backBtn = new QPushButton("← Back to Dashboard", this);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet("background: transparent; border: none; font-weight: bold; color: #4F46E5;");
    connect(backBtn, &QPushButton::clicked, this, &AdminView::backRequested);

    auto* titleLabel = new QLabel("Curriculum Administration & Governance", this);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #0F172A;");

    topBar->addWidget(backBtn);
    topBar->addSpacing(16);
    topBar->addWidget(titleLabel);
    topBar->addStretch();
    rootLayout->addLayout(topBar);

    statsLabel_ = new QLabel("System Telemetry: Loading...", this);
    statsLabel_->setStyleSheet("background: #F1F5F9; border-radius: 8px; padding: 12px; font-weight: 500;");
    rootLayout->addWidget(statsLabel_);

    questionsTable_ = new QTableWidget(this);
    questionsTable_->setColumnCount(5);
    questionsTable_->setHorizontalHeaderLabels({"ID", "Subject", "Topic", "Difficulty", "Active Status"});
    questionsTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    questionsTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    questionsTable_->verticalHeader()->setVisible(false);
    rootLayout->addWidget(questionsTable_);

    auto* bBar = new QHBoxLayout();
    softDeleteBtn_ = new QPushButton("Deactivate Selected Question (Soft Delete)", this);
    softDeleteBtn_->setCursor(Qt::PointingHandCursor);
    softDeleteBtn_->setStyleSheet("background: #DC2626; color: white; border-radius: 6px; padding: 8px 16px; font-weight: bold;");

    connect(softDeleteBtn_, &QPushButton::clicked, this, [this]() {
        int row = questionsTable_->currentRow();
        if (row >= 0 && currentAdmin_) {
            QString qId = questionsTable_->item(row, 0)->text();
            try {
                ctx_->adminService->softDeleteQuestion(*currentAdmin_, qId.toStdString());
                QMessageBox::information(this, "Success", "Question marked as inactive.");
                loadQuestions();
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "Error", e.what());
            }
        }
    });

    bBar->addStretch();
    bBar->addWidget(softDeleteBtn_);
    rootLayout->addLayout(bBar);
}

void AdminView::refreshAdminData(std::shared_ptr<Admin> admin) {
    currentAdmin_ = admin;
    if (!currentAdmin_) return;

    try {
        std::string today = ctx_->clock ? ctx_->clock->getCurrentDate() : "2026-10-12";
        auto stats = ctx_->adminService->getUserStats(*currentAdmin_, today);
        statsLabel_->setText(QString("Total Students: %1 | Active Today: %2 | Questions Managed: Active Curriculum")
            .arg(stats.totalUsers).arg(stats.activeTodayCount));
    } catch (...) {
        statsLabel_->setText("Telemetry unavailable.");
    }

    loadQuestions();
}

void AdminView::loadQuestions() {
    auto questions = ctx_->questionRepo->getAll();
    questionsTable_->setRowCount(static_cast<int>(questions.size()));

    for (size_t i = 0; i < questions.size(); ++i) {
        const auto& q = questions[i];
        questionsTable_->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::fromStdString(q->getId())));
        questionsTable_->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::fromStdString(q->getSubject())));
        questionsTable_->setItem(static_cast<int>(i), 2, new QTableWidgetItem(QString::fromStdString(q->getTopic())));
        questionsTable_->setItem(static_cast<int>(i), 3, new QTableWidgetItem(difficultyToString(q->getDifficulty()).c_str()));
        questionsTable_->setItem(static_cast<int>(i), 4, new QTableWidgetItem(q->isActive() ? "Active" : "Inactive"));
    }
}
