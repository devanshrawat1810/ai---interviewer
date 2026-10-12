#include "gui/views/ProfileView.h"
#include "gui/ThemeTokens.h"
#include "core/Student.h"
#include <QDate>
#include <set>

ProfileView::ProfileView(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void ProfileView::setupUi() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(32, 24, 32, 24);
    rootLayout->setSpacing(20);

    // Header with Back button
    auto* topBar = new QHBoxLayout();
    auto* backBtn = new QPushButton("← Back to Dashboard", this);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet("background: transparent; border: none; font-weight: bold; color: #4F46E5;");
    connect(backBtn, &QPushButton::clicked, this, &ProfileView::backRequested);

    auto* titleLabel = new QLabel("Student Profile & Streak Calendar", this);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #0F172A;");

    topBar->addWidget(backBtn);
    topBar->addSpacing(16);
    topBar->addWidget(titleLabel);
    topBar->addStretch();
    rootLayout->addLayout(topBar);

    auto* mainRow = new QHBoxLayout();
    mainRow->setSpacing(24);

    // Left Column: Profile Card & Editor
    auto* leftCard = new Card(this);
    leftCard->setFixedWidth(380);
    auto* leftLayout = new QVBoxLayout(leftCard);
    leftLayout->setSpacing(14);

    avatarLabel_ = new QLabel("JD", leftCard);
    avatarLabel_->setFixedSize(64, 64);
    avatarLabel_->setAlignment(Qt::AlignCenter);
    avatarLabel_->setStyleSheet(QString("background: %1; color: white; font-size: 22px; font-weight: bold; border-radius: 32px;")
        .arg(ThemeTokens::ColorPrimary));

    nameLabel_ = new QLabel("Jane Doe", leftCard);
    nameLabel_->setStyleSheet("font-size: 18px; font-weight: bold; color: #1E293B;");
    emailLabel_ = new QLabel("jane@college.edu", leftCard);
    emailLabel_->setStyleSheet("color: #64748B; font-size: 13px;");

    collegeField_ = new FormField("College / Institution", leftCard);
    branchField_ = new FormField("Department / Branch", leftCard);

    saveBtn_ = new PrimaryButton("Save Profile Details", leftCard);
    connect(saveBtn_, &QPushButton::clicked, this, [this]() {
        emit profileUpdated(collegeField_->text(), branchField_->text());
    });

    leftLayout->addWidget(avatarLabel_);
    leftLayout->addWidget(nameLabel_);
    leftLayout->addWidget(emailLabel_);
    leftLayout->addSpacing(10);
    leftLayout->addWidget(collegeField_);
    leftLayout->addWidget(branchField_);
    leftLayout->addWidget(saveBtn_);
    leftLayout->addStretch();

    // Right Column: Streak Calendar & Badges
    auto* rightCard = new Card(this);
    auto* rightLayout = new QVBoxLayout(rightCard);
    rightLayout->setSpacing(16);

    auto* calTitle = new QLabel("Monthly Practice Streak Calendar (October 2026)", rightCard);
    calTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1E293B;");

    auto* calContainer = new QWidget(rightCard);
    calendarGrid_ = new QGridLayout(calContainer);
    calendarGrid_->setSpacing(6);

    auto* badgeTitle = new QLabel("Earned Placement Badges", rightCard);
    badgeTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1E293B;");

    auto* badgesContainer = new QWidget(rightCard);
    badgesLayout_ = new QVBoxLayout(badgesContainer);

    rightLayout->addWidget(calTitle);
    rightLayout->addWidget(calContainer);
    rightLayout->addWidget(badgeTitle);
    rightLayout->addWidget(badgesContainer);
    rightLayout->addStretch();

    mainRow->addWidget(leftCard);
    mainRow->addWidget(rightCard, 1);

    rootLayout->addLayout(mainRow);
}

void ProfileView::setStudentData(std::shared_ptr<Student> student) {
    currentStudent_ = student;
    if (!currentStudent_) return;

    QString name = QString::fromStdString(currentStudent_->getName());
    nameLabel_->setText(name);
    emailLabel_->setText(QString::fromStdString(currentStudent_->getEmail()));

    QString initials = name.isEmpty() ? "ST" : name.left(1).toUpper();
    avatarLabel_->setText(initials);

    collegeField_->setText(QString::fromStdString(currentStudent_->getCollege()));
    branchField_->setText(QString::fromStdString(currentStudent_->getBranch()));

    populateCalendar(currentStudent_->getActivityDates());

    // Badges
    QLayoutItem* item;
    while ((item = badgesLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    auto badges = currentStudent_->getBadges();
    if (badges.empty()) {
        auto* noB = new QLabel("No badges unlocked yet. Complete mock interviews and daily challenges to unlock!", this);
        noB->setStyleSheet("color: #94A3B8; font-style: italic;");
        badgesLayout_->addWidget(noB);
    } else {
        for (const auto& b : badges) {
            auto* bLbl = new QLabel(QString("🏆 %1").arg(QString::fromStdString(b)), this);
            bLbl->setStyleSheet(QString("background: #FEF3C7; color: #B45309; border-radius: 6px; padding: 6px 12px; font-weight: 600; font-size: 13px;"));
            badgesLayout_->addWidget(bLbl);
        }
    }
}

void ProfileView::populateCalendar(const std::vector<std::string>& activeDates) {
    QLayoutItem* item;
    while ((item = calendarGrid_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    std::set<int> activeDays;
    for (const auto& d : activeDates) {
        QDate qd = QDate::fromString(QString::fromStdString(d), "yyyy-MM-dd");
        if (qd.isValid() && qd.month() == 10) {
            activeDays.insert(qd.day());
        }
    }

    QString daysOfWeek[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    for (int i = 0; i < 7; ++i) {
        auto* header = new QLabel(daysOfWeek[i], this);
        header->setAlignment(Qt::AlignCenter);
        header->setStyleSheet("font-size: 11px; font-weight: bold; color: #94A3B8;");
        calendarGrid_->addWidget(header, 0, i);
    }

    for (int day = 1; day <= 31; ++day) {
        int row = (day + 2) / 7 + 1; // Approx offset for Oct 2026
        int col = (day + 2) % 7;

        auto* dayLbl = new QLabel(QString::number(day), this);
        dayLbl->setFixedSize(36, 36);
        dayLbl->setAlignment(Qt::AlignCenter);

        if (activeDays.count(day)) {
            dayLbl->setStyleSheet(QString("background: %1; color: white; border-radius: 18px; font-weight: bold;").arg(ThemeTokens::ColorPrimary));
        } else {
            dayLbl->setStyleSheet("background: #F1F5F9; color: #64748B; border-radius: 18px;");
        }
        calendarGrid_->addWidget(dayLbl, row, col);
    }
}
