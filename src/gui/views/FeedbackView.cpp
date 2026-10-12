#include "gui/views/FeedbackView.h"
#include "gui/ThemeTokens.h"
#include <QScrollArea>

FeedbackView::FeedbackView(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void FeedbackView::setupUi() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(32, 24, 32, 24);
    rootLayout->setSpacing(20);

    auto* titleLabel = new QLabel("Interview Performance & Feedback Report", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0F172A;");
    rootLayout->addWidget(titleLabel);

    noticeBanner_ = new QLabel(this);
    noticeBanner_->setStyleSheet("background: #EEF2FF; color: #4338CA; border: 1px solid #C7D2FE; border-radius: 8px; padding: 10px; font-weight: 500;");
    rootLayout->addWidget(noticeBanner_);

    // Hero Score Card
    auto* heroCard = new Card(this);
    auto* heroLayout = new QHBoxLayout(heroCard);

    auto* scoreCol = new QVBoxLayout();
    scoreHeroLabel_ = new QLabel("80 / 100", heroCard);
    scoreHeroLabel_->setStyleSheet("font-size: 36px; font-weight: bold; color: #4F46E5;");
    percentLabel_ = new QLabel("Score Percentage: 80.0%", heroCard);
    percentLabel_->setStyleSheet("font-size: 15px; color: #64748B;");
    scoreCol->addWidget(scoreHeroLabel_);
    scoreCol->addWidget(percentLabel_);

    auto* topicsCol = new QVBoxLayout();
    strongestTopicLabel_ = new QLabel("Strongest: Object-Oriented Design", heroCard);
    strongestTopicLabel_->setStyleSheet("color: #16A34A; font-weight: 600; font-size: 14px;");
    weakestTopicLabel_ = new QLabel("Focus Area: Memory Allocation", heroCard);
    weakestTopicLabel_->setStyleSheet("color: #D97706; font-weight: 600; font-size: 14px;");
    topicsCol->addWidget(strongestTopicLabel_);
    topicsCol->addWidget(weakestTopicLabel_);

    heroLayout->addLayout(scoreCol);
    heroLayout->addStretch();
    heroLayout->addLayout(topicsCol);

    rootLayout->addWidget(heroCard);

    // Score Chart Card
    auto* chartCard = new Card(this);
    auto* chartLayout = new QVBoxLayout(chartCard);
    auto* chartTitle = new QLabel("Score Distribution by Difficulty", chartCard);
    chartTitle->setStyleSheet("font-size: 15px; font-weight: bold; color: #1E293B;");
    chartWidget_ = new ChartWidget(chartCard);
    chartLayout->addWidget(chartTitle);
    chartLayout->addWidget(chartWidget_);
    rootLayout->addWidget(chartCard);

    // Qualitative AI Summary Card
    auto* aiCard = new Card(this);
    auto* aiLayout = new QVBoxLayout(aiCard);
    auto* aiTitle = new QLabel("Qualitative AI Assessment", aiCard);
    aiTitle->setStyleSheet("font-size: 15px; font-weight: bold; color: #1E293B;");
    aiSummaryLabel_ = new QLabel(aiCard);
    aiSummaryLabel_->setWordWrap(true);
    aiSummaryLabel_->setStyleSheet("color: #334155; font-size: 14px; line-height: 1.5;");
    aiLayout->addWidget(aiTitle);
    aiLayout->addWidget(aiSummaryLabel_);
    rootLayout->addWidget(aiCard);

    // Recommendations Layout
    auto* recCard = new Card(this);
    suggestionsLayout_ = new QVBoxLayout(recCard);
    auto* recTitle = new QLabel("Actionable Next Steps & Recommendations", recCard);
    recTitle->setStyleSheet("font-size: 15px; font-weight: bold; color: #1E293B;");
    suggestionsLayout_->addWidget(recTitle);
    rootLayout->addWidget(recCard);

    // Bottom Action Buttons
    auto* bBar = new QHBoxLayout();
    retryBtn_ = new QPushButton("Try Another Session", this);
    retryBtn_->setCursor(Qt::PointingHandCursor);
    retryBtn_->setStyleSheet("background: transparent; border: 1px solid #CBD5E1; border-radius: 8px; padding: 10px 18px; font-weight: 600;");
    connect(retryBtn_, &QPushButton::clicked, this, &FeedbackView::retryInterviewClicked);

    returnBtn_ = new PrimaryButton("Return to Dashboard", this);
    returnBtn_->setFixedWidth(200);
    connect(returnBtn_, &QPushButton::clicked, this, &FeedbackView::returnToDashboardClicked);

    bBar->addWidget(retryBtn_);
    bBar->addStretch();
    bBar->addWidget(returnBtn_);

    rootLayout->addLayout(bBar);
}

void FeedbackView::setReport(const FeedbackReport& report) {
    scoreHeroLabel_->setText(QString("%1 / %2").arg(report.totalScore).arg(report.maxScore));
    percentLabel_->setText(QString("Final Percentage: %1%").arg(QString::number(report.percentage, 'f', 1)));

    noticeBanner_->setText(report.isAiGenerated ?
        "✨ AI-Generated Feedback: Multidimensional qualitative evaluation active." :
        "📋 Standard Rule-Based Feedback: Deterministic evaluation applied.");

    strongestTopicLabel_->setText(QString("Strongest Topic: %1").arg(QString::fromStdString(report.strongestTopic)));
    weakestTopicLabel_->setText(QString("Focus Topic: %1").arg(QString::fromStdString(report.weakestTopic)));

    chartWidget_->setData(report.scoreByDifficulty, report.maxByDifficulty);

    if (report.isAiGenerated && !report.overallSummary.empty()) {
        aiSummaryLabel_->setText(QString::fromStdString(report.overallSummary));
    } else {
        aiSummaryLabel_->setText("Candidate demonstrated good baseline technical recall across the curriculum.");
    }

    // Clear and populate suggestions
    for (const auto& sug : report.suggestions) {
        auto* sLbl = new QLabel(QString("• %1").arg(QString::fromStdString(sug)), this);
        sLbl->setStyleSheet("color: #475569; font-size: 13px;");
        suggestionsLayout_->addWidget(sLbl);
    }
}
