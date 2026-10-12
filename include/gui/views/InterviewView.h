#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include "gui/presenters/InterviewPresenter.h"
#include "gui/widgets/Card.h"
#include "gui/widgets/DifficultyBadge.h"
#include "gui/widgets/ProgressRing.h"
#include "gui/widgets/PrimaryButton.h"
#include "gui/widgets/QuestionViewFactory.h"

// OOP Concept: Passive View in Model-View-Presenter
// Central mock interview screen with countdown timer, question views, and submission feedback banners.
class InterviewView : public QWidget {
    Q_OBJECT

public:
    explicit InterviewView(InterviewPresenter* presenter, QWidget* parent = nullptr);

public slots:
    void onQuestionPresented(int index, int total, std::shared_ptr<Question> question, int timeLimit);
    void onTimerTick(int remaining, int total);
    void onAnswerSubmitted(bool isCorrect, const QString& explanation);
    void onAnswerReviewReady(const QString& feedback, int rating, const QString& improvedAnswer);

signals:
    void leaveRequested();
    void interviewFinished(const FeedbackReport& report);

private:
    InterviewPresenter* presenter_;

    // Top Bar
    QLabel* counterLabel_;
    DifficultyBadge* diffBadge_;
    QLabel* topicChip_;
    ProgressRing* timerRing_;
    QPushButton* leaveBtn_;
    QProgressBar* sessionProgress_;

    // Question Container
    Card* questionCard_;
    QLabel* questionTextLabel_;
    QVBoxLayout* inputContainerLayout_;
    BaseQuestionWidget* currentInputWidget_{nullptr};

    // Review / Coaching Panel
    Card* coachingCard_;
    QLabel* coachingLabel_;

    // Bottom Action Bar
    QLabel* resultBanner_;
    PrimaryButton* submitBtn_;
    QPushButton* skipBtn_;
    QPushButton* reviewBtn_;

    void setupUi();
};
