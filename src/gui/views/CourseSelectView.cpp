#include "gui/views/CourseSelectView.h"
#include "gui/ThemeTokens.h"
#include <QProgressBar>

CourseSelectView::CourseSelectView(CourseSelectPresenter* presenter, QWidget* parent)
    : QWidget(parent), presenter_(presenter) {
    setupUi();
    connect(presenter_, &CourseSelectPresenter::courseDataLoaded, this, &CourseSelectView::updateCourseData);
}

void CourseSelectView::setupUi() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(32, 24, 32, 24);
    rootLayout->setSpacing(16);

    // Top Header with Back button and Breadcrumb
    auto* topBar = new QHBoxLayout();
    auto* backBtn = new QPushButton("← Back to Dashboard", this);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet("background: transparent; border: none; font-weight: bold; color: #4F46E5;");
    connect(backBtn, &QPushButton::clicked, this, &CourseSelectView::backRequested);

    breadcrumbLabel_ = new QLabel("Courses > Computer Science Track", this);
    breadcrumbLabel_->setStyleSheet("color: #64748B; font-size: 14px;");

    topBar->addWidget(backBtn);
    topBar->addSpacing(16);
    topBar->addWidget(breadcrumbLabel_);
    topBar->addStretch();

    rootLayout->addLayout(topBar);

    auto* titleLabel = new QLabel("Select Subject & Practice Mode", this);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #0F172A;");
    rootLayout->addWidget(titleLabel);

    subjectsLayout_ = new QVBoxLayout();
    subjectsLayout_->setSpacing(12);
    rootLayout->addLayout(subjectsLayout_);

    rootLayout->addStretch();
}

void CourseSelectView::updateCourseData(const CourseProgressData& data) {
    breadcrumbLabel_->setText(QString("Courses > %1").arg(data.courseName));

    // Clear previous items
    QLayoutItem* item;
    while ((item = subjectsLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (const auto& sub : data.subjects) {
        auto* card = new Card(this);
        auto* cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(16, 12, 16, 12);

        auto* col = new QVBoxLayout();
        auto* nameLbl = new QLabel(sub.name, card);
        nameLbl->setStyleSheet("font-size: 16px; font-weight: bold; color: #1E293B;");
        auto* countLbl = new QLabel(QString("%1 questions available").arg(sub.totalQuestions), card);
        countLbl->setStyleSheet("color: #64748B; font-size: 13px;");

        col->addWidget(nameLbl);
        col->addWidget(countLbl);

        auto* btnMock = new QPushButton("Start Mock (10 Ramp)", card);
        btnMock->setCursor(Qt::PointingHandCursor);
        btnMock->setStyleSheet(QString("background: %1; color: white; border-radius: 6px; padding: 8px 14px; font-weight: 600;")
            .arg(ThemeTokens::ColorPrimary));

        auto* btnPractice = new QPushButton("Practice (Adaptive)", card);
        btnPractice->setCursor(Qt::PointingHandCursor);
        btnPractice->setStyleSheet("background: transparent; border: 1px solid #CBD5E1; border-radius: 6px; padding: 8px 14px;");

        QString sName = sub.name;
        connect(btnMock, &QPushButton::clicked, this, [this, sName]() {
            emit subjectSelected(sName, "mock");
        });
        connect(btnPractice, &QPushButton::clicked, this, [this, sName]() {
            emit subjectSelected(sName, "practice");
        });

        cardLayout->addLayout(col);
        cardLayout->addStretch();
        cardLayout->addWidget(btnPractice);
        cardLayout->addWidget(btnMock);

        subjectsLayout_->addWidget(card);
    }
}
