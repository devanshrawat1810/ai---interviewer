#pragma once

#include <QObject>
#include <QTimer>
#include <memory>
#include <QString>
#include "feedback/FeedbackGenerator.h"

class ServiceContext;
class InterviewSession;
class Question;

// OOP Concept: Presenter in Model-View-Presenter
// Drives the 10-question interview timeline, countdown timer, question transitions, and AI coaching.
class InterviewPresenter : public QObject {
    Q_OBJECT

public:
    explicit InterviewPresenter(std::shared_ptr<ServiceContext> ctx, QObject* parent = nullptr);

    void startSession(std::shared_ptr<InterviewSession> session);
    void submitAnswer(const QString& answer);
    void skipQuestion();
    void requestAnswerReview(const QString& answer);

signals:
    void questionPresented(int index, int total, std::shared_ptr<Question> question, int timeLimitSeconds);
    void timerTick(int secondsRemaining, int totalSeconds);
    void timerExpired();
    void answerSubmitted(bool isCorrect, const QString& explanation);
    void sessionCompleted(const FeedbackReport& report);
    void answerReviewReady(const QString& feedback, int rating, const QString& improvedAnswer);

private slots:
    void onTick();

private:
    std::shared_ptr<ServiceContext> ctx_;
    std::shared_ptr<InterviewSession> session_;
    std::shared_ptr<Question> currentQuestion_;
    QTimer* timer_;
    int secondsRemaining_{60};
    int totalQuestionSeconds_{60};
    std::chrono::steady_clock::time_point questionStartTime_;

    void loadNextQuestion();
};
