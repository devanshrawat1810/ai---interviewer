#include "gui/presenters/InterviewPresenter.h"
#include "gui/ServiceContext.h"
#include "interview/InterviewSession.h"
#include <QtConcurrent>

InterviewPresenter::InterviewPresenter(std::shared_ptr<ServiceContext> ctx, QObject* parent)
    : QObject(parent), ctx_(std::move(ctx)) {
    timer_ = new QTimer(this);
    timer_->setInterval(1000);
    connect(timer_, &QTimer::timeout, this, &InterviewPresenter::onTick);
}

void InterviewPresenter::startSession(std::shared_ptr<InterviewSession> session) {
    session_ = std::move(session);
    if (!session_) return;

    session_->start();
    loadNextQuestion();
}

void InterviewPresenter::loadNextQuestion() {
    if (!session_ || session_->isCompleted()) {
        timer_->stop();
        // Generate feedback off-thread
        (void)QtConcurrent::run([this]() {
            FeedbackReport report = ctx_->aiFeedback->generateFeedback(*session_);
            QMetaObject::invokeMethod(this, [this, report]() {
                emit sessionCompleted(report);
            }, Qt::QueuedConnection);
        });
        return;
    }

    currentQuestion_ = session_->getNextQuestion();
    if (!currentQuestion_) {
        session_->finish();
        loadNextQuestion();
        return;
    }

    totalQuestionSeconds_ = InterviewSession::getTimeLimitForDifficulty(currentQuestion_->getDifficulty());
    secondsRemaining_ = totalQuestionSeconds_;
    questionStartTime_ = std::chrono::steady_clock::now();

    int idx = session_->getStandardQuestionsAttempted() + 1;
    int total = session_->getTargetQuestionCount();

    emit questionPresented(idx, total, currentQuestion_, totalQuestionSeconds_);
    emit timerTick(secondsRemaining_, totalQuestionSeconds_);

    timer_->start();
}

void InterviewPresenter::onTick() {
    if (secondsRemaining_ > 0) {
        secondsRemaining_--;
        emit timerTick(secondsRemaining_, totalQuestionSeconds_);
        if (secondsRemaining_ == 0) {
            timer_->stop();
            emit timerExpired();
            submitAnswer(""); // Auto-submit blank on timeout
        }
    }
}

void InterviewPresenter::submitAnswer(const QString& answer) {
    timer_->stop();
    int elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - questionStartTime_).count());

    bool correct = session_->submitAnswer(currentQuestion_, answer.toStdString(), elapsed);
    QString explanation = QString::fromStdString(currentQuestion_->getExplanation());

    emit answerSubmitted(correct, explanation);

    // Proceed to next question after brief pause
    QTimer::singleShot(1500, this, &InterviewPresenter::loadNextQuestion);
}

void InterviewPresenter::skipQuestion() {
    submitAnswer("");
}

void InterviewPresenter::requestAnswerReview(const QString& answer) {
    if (!currentQuestion_) return;

    std::string qText = currentQuestion_->getText();
    std::string ans = answer.toStdString();
    std::string uid = session_->getUserId();

    (void)QtConcurrent::run([this, uid, qText, ans]() {
        auto res = ctx_->answerReviewer->reviewAnswer(uid, qText, ans);
        if (res.success) {
            QMetaObject::invokeMethod(this, [this, res]() {
                emit answerReviewReady(
                    QString::fromStdString(res.feedback),
                    res.rating,
                    QString::fromStdString(res.improvedAnswer)
                );
            }, Qt::QueuedConnection);
        }
    });
}
