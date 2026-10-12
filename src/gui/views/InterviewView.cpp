#include "gui/views/InterviewView.h"
#include "gui/ThemeTokens.h"
#include <QMessageBox>

InterviewView::InterviewView(InterviewPresenter* presenter, QWidget* parent)
    : QWidget(parent), presenter_(presenter) {
    setupUi();

    connect(presenter_, &InterviewPresenter::questionPresented, this, &InterviewView::onQuestionPresented);
    connect(presenter_, &InterviewPresenter::timerTick, this, &InterviewView::onTimerTick);
    connect(presenter_, &InterviewPresenter::answerSubmitted, this, &InterviewView::onAnswerSubmitted);
    connect(presenter_, &InterviewPresenter::sessionCompleted, this, &InterviewView::interviewFinished);
    connect(presenter_, &InterviewPresenter::answerReviewReady, this, &InterviewView::onAnswerReviewReady);
}

void InterviewView::setupUi() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(32, 20, 32, 20);
    rootLayout->setSpacing(16);

    // Top Header
    auto* topBar = new QHBoxLayout();

    counterLabel_ = new QLabel("Question 1 of 10", this);
    counterLabel_->setStyleSheet("font-size: 16px; font-weight: bold; color: #1E293B;");

    diffBadge_ = new DifficultyBadge(Difficulty::EASY, this);

    topicChip_ = new QLabel("Object-Oriented Programming", this);
    topicChip_->setStyleSheet("background: #F1F5F9; color: #475569; border-radius: 6px; padding: 4px 10px; font-size: 12px; font-weight: 500;");

    timerRing_ = new ProgressRing(this);

    leaveBtn_ = new QPushButton("Leave Interview", this);
    leaveBtn_->setCursor(Qt::PointingHandCursor);
    leaveBtn_->setStyleSheet("background: transparent; color: #DC2626; border: 1px solid #FECACA; border-radius: 6px; padding: 6px 12px;");

    connect(leaveBtn_, &QPushButton::clicked, this, [this]() {
        auto res = QMessageBox::question(this, "Leave Interview",
            "Are you sure you want to leave? Your in-progress session will be finished.",
            QMessageBox::Yes | QMessageBox::No);
        if (res == QMessageBox::Yes) {
            emit leaveRequested();
        }
    });

    topBar->addWidget(counterLabel_);
    topBar->addSpacing(10);
    topBar->addWidget(diffBadge_);
    topBar->addSpacing(10);
    topBar->addWidget(topicChip_);
    topBar->addStretch();
    topBar->addWidget(timerRing_);
    topBar->addSpacing(16);
    topBar->addWidget(leaveBtn_);

    rootLayout->addLayout(topBar);

    sessionProgress_ = new QProgressBar(this);
    sessionProgress_->setFixedHeight(6);
    sessionProgress_->setTextVisible(false);
    sessionProgress_->setRange(0, 10);
    sessionProgress_->setValue(1);
    rootLayout->addWidget(sessionProgress_);

    // Question Card
    questionCard_ = new Card(this);
    auto* qCardLayout = new QVBoxLayout(questionCard_);
    qCardLayout->setSpacing(16);

    questionTextLabel_ = new QLabel("Loading question...", questionCard_);
    questionTextLabel_->setStyleSheet("font-size: 16px; font-weight: 600; line-height: 1.5; color: #0F172A;");
    questionTextLabel_->setWordWrap(true);

    inputContainerLayout_ = new QVBoxLayout();

    qCardLayout->addWidget(questionTextLabel_);
    qCardLayout->addLayout(inputContainerLayout_);

    rootLayout->addWidget(questionCard_);

    // Coaching Panel (Hidden by default)
    coachingCard_ = new Card(this);
    coachingCard_->setStyleSheet("background-color: #F0FDF4; border: 1px solid #BBF7D0;");
    auto* coachLayout = new QVBoxLayout(coachingCard_);
    coachingLabel_ = new QLabel(coachingCard_);
    coachingLabel_->setWordWrap(true);
    coachingLabel_->setStyleSheet("color: #166534; font-size: 13px;");
    coachLayout->addWidget(coachingLabel_);
    coachingCard_->setVisible(false);
    rootLayout->addWidget(coachingCard_);

    // Result Banner (Appears after submission)
    resultBanner_ = new QLabel(this);
    resultBanner_->setStyleSheet("border-radius: 8px; padding: 10px 14px; font-weight: bold; font-size: 14px;");
    resultBanner_->setVisible(false);
    resultBanner_->setWordWrap(true);
    rootLayout->addWidget(resultBanner_);

    // Bottom Action Bar
    auto* bottomBar = new QHBoxLayout();

    reviewBtn_ = new QPushButton("✨ Review My Answer (AI)", this);
    reviewBtn_->setCursor(Qt::PointingHandCursor);
    reviewBtn_->setStyleSheet("background: transparent; color: #4F46E5; border: 1px solid #C7D2FE; border-radius: 6px; padding: 8px 14px;");
    reviewBtn_->setVisible(false);

    connect(reviewBtn_, &QPushButton::clicked, this, [this]() {
        if (currentInputWidget_) {
            presenter_->requestAnswerReview(currentInputWidget_->getAnswer());
        }
    });

    skipBtn_ = new QPushButton("Skip Question", this);
    skipBtn_->setCursor(Qt::PointingHandCursor);
    skipBtn_->setStyleSheet("background: transparent; border: 1px solid #CBD5E1; border-radius: 6px; padding: 8px 14px;");
    connect(skipBtn_, &QPushButton::clicked, presenter_, &InterviewPresenter::skipQuestion);

    submitBtn_ = new PrimaryButton("Submit Answer (Ctrl+Enter)", this);
    submitBtn_->setFixedWidth(200);

    connect(submitBtn_, &QPushButton::clicked, this, [this]() {
        if (currentInputWidget_) {
            presenter_->submitAnswer(currentInputWidget_->getAnswer());
        }
    });

    bottomBar->addWidget(reviewBtn_);
    bottomBar->addStretch();
    bottomBar->addWidget(skipBtn_);
    bottomBar->addWidget(submitBtn_);

    rootLayout->addLayout(bottomBar);
}

void InterviewView::onQuestionPresented(int index, int total, std::shared_ptr<Question> question, int timeLimit) {
    counterLabel_->setText(QString("Question %1 of %2").arg(index).arg(total));
    sessionProgress_->setValue(index);
    diffBadge_->setDifficulty(question->getDifficulty());
    topicChip_->setText(QString::fromStdString(question->getTopic()));
    questionTextLabel_->setText(QString::fromStdString(question->getText()));

    resultBanner_->setVisible(false);
    coachingCard_->setVisible(false);
    submitBtn_->setEnabled(true);
    skipBtn_->setEnabled(true);

    // Show AI Review button for text questions
    reviewBtn_->setVisible(question->getType() == "text");

    // Clear old input widget
    if (currentInputWidget_) {
        inputContainerLayout_->removeWidget(currentInputWidget_);
        currentInputWidget_->deleteLater();
        currentInputWidget_ = nullptr;
    }

    currentInputWidget_ = QuestionViewFactory::createView(question, questionCard_);
    inputContainerLayout_->addWidget(currentInputWidget_);
}

void InterviewView::onTimerTick(int remaining, int total) {
    timerRing_->setTime(remaining, total);
}

void InterviewView::onAnswerSubmitted(bool isCorrect, const QString& explanation) {
    submitBtn_->setEnabled(false);
    skipBtn_->setEnabled(false);

    if (isCorrect) {
        resultBanner_->setText("✅ Correct Answer! Excellent job.");
        resultBanner_->setStyleSheet(QString("background: #DCFCE7; color: %1; border: 1px solid #86EFAC; border-radius: 8px; padding: 10px 14px; font-weight: bold;")
            .arg(ThemeTokens::ColorSuccess));
    } else {
        resultBanner_->setText(QString("❌ Not quite. %1").arg(explanation));
        resultBanner_->setStyleSheet(QString("background: #FEF2F2; color: %1; border: 1px solid #FCA5A5; border-radius: 8px; padding: 10px 14px; font-weight: bold;")
            .arg(ThemeTokens::ColorDanger));
    }
    resultBanner_->setVisible(true);
}

void InterviewView::onAnswerReviewReady(const QString& feedback, int rating, const QString& improvedAnswer) {
    coachingLabel_->setText(QString("<b>AI Coaching (Rating: %1/5):</b> %2<br><br><b>Suggested Model Answer:</b> %3")
        .arg(rating).arg(feedback).arg(improvedAnswer));
    coachingCard_->setVisible(true);
}
