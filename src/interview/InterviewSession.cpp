#include "interview/InterviewSession.h"
#include <algorithm>
#include <utility>

// OOP Concept: Encapsulation & Orchestration
// Implementation of InterviewSession managing timed question flow and weighted scoring.

InterviewSession::InterviewSession(std::string id,
                                   std::string userId,
                                   std::string subjectId,
                                   std::string mode,
                                   std::unique_ptr<QuestionSelectionStrategy> strategy,
                                   std::vector<std::shared_ptr<Question>> questionPool,
                                   int targetQuestionCount)
    : id_(std::move(id)),
      userId_(std::move(userId)),
      subjectId_(std::move(subjectId)),
      mode_(std::move(mode)),
      targetQuestionCount_(targetQuestionCount),
      strategy_(std::move(strategy)),
      questionPool_(std::move(questionPool)) {}

void InterviewSession::start() {
    startTime_ = std::chrono::system_clock::now();
    isCompleted_ = false;
    records_.clear();
    resultHistory_.clear();
    if (strategy_) {
        strategy_->reset();
    }
}

void InterviewSession::finish() {
    endTime_ = std::chrono::system_clock::now();
    isCompleted_ = true;
}

int InterviewSession::getTimeLimitForDifficulty(Difficulty diff) {
    switch (diff) {
        case Difficulty::EASY:   return 60;  // 1 minute
        case Difficulty::MEDIUM: return 120; // 2 minutes
        case Difficulty::HARD:   return 240; // 4 minutes
    }
    return 60;
}

int InterviewSession::getWeightForDifficulty(Difficulty diff) {
    switch (diff) {
        case Difficulty::EASY:   return 1;
        case Difficulty::MEDIUM: return 2;
        case Difficulty::HARD:   return 3;
    }
    return 1;
}

std::shared_ptr<Question> InterviewSession::getNextQuestion() {
    if (standardQuestionsAttempted_ >= targetQuestionCount_) {
        return nullptr;
    }

    std::vector<std::shared_ptr<Question>> asked;
    asked.reserve(records_.size());
    for (const auto& rec : records_) {
        asked.push_back(rec.question);
    }

    int nextQNum = standardQuestionsAttempted_ + 1;
    return strategy_->selectNextQuestion(nextQNum, questionPool_, asked, resultHistory_);
}

bool InterviewSession::submitAnswer(std::shared_ptr<Question> question,
                                    const std::string& answer,
                                    int timeTakenSeconds) {
    if (!question) return false;

    Difficulty diff = question->getDifficulty();
    int limit = getTimeLimitForDifficulty(diff);
    int weight = getWeightForDifficulty(diff);

    bool correct = false;
    // Check if within time limit; if timed out, answer is marked wrong
    if (timeTakenSeconds <= limit) {
        correct = question->evaluate(answer);
    }

    SessionQuestionRecord rec;
    rec.question = question;
    rec.userResponse = answer;
    rec.isCorrect = correct;
    rec.timeTakenSeconds = timeTakenSeconds;
    rec.timeLimitSeconds = limit;
    rec.maxPoints = weight;
    rec.pointsEarned = correct ? weight : 0;

    records_.push_back(rec);
    resultHistory_.push_back(correct);
    standardQuestionsAttempted_++;

    if (standardQuestionsAttempted_ >= targetQuestionCount_) {
        finish();
    }

    return correct;
}

bool InterviewSession::submitFollowUpAnswer(std::shared_ptr<Question> question,
                                            const std::string& answer,
                                            int timeTakenSeconds) {
    if (!question) return false;

    Difficulty diff = question->getDifficulty();
    int limit = getTimeLimitForDifficulty(diff);
    int fullWeight = getWeightForDifficulty(diff);
    int halfPoints = std::max(1, fullWeight / 2);

    bool correct = false;
    if (timeTakenSeconds <= limit) {
        correct = question->evaluate(answer);
    }

    SessionQuestionRecord rec;
    rec.question = question;
    rec.userResponse = answer;
    rec.isCorrect = correct;
    rec.timeTakenSeconds = timeTakenSeconds;
    rec.timeLimitSeconds = limit;
    rec.maxPoints = halfPoints;
    rec.pointsEarned = correct ? halfPoints : 0;

    records_.push_back(rec);
    followUpsAsked_++;
    return correct;
}

const std::string& InterviewSession::getId() const noexcept {
    return id_;
}

const std::string& InterviewSession::getUserId() const noexcept {
    return userId_;
}

const std::string& InterviewSession::getSubjectId() const noexcept {
    return subjectId_;
}

const std::string& InterviewSession::getMode() const noexcept {
    return mode_;
}

int InterviewSession::getTotalScore() const noexcept {
    int total = 0;
    for (const auto& r : records_) {
        total += r.pointsEarned;
    }
    return total;
}

int InterviewSession::getMaxPossibleScore() const noexcept {
    int maxPts = 0;
    for (const auto& r : records_) {
        maxPts += r.maxPoints;
    }
    return maxPts;
}

int InterviewSession::getCorrectCount() const noexcept {
    int count = 0;
    for (const auto& r : records_) {
        if (r.isCorrect) count++;
    }
    return count;
}

int InterviewSession::getQuestionsAttempted() const noexcept {
    return static_cast<int>(records_.size());
}

int InterviewSession::getTargetQuestionCount() const noexcept {
    return targetQuestionCount_;
}

bool InterviewSession::isCompleted() const noexcept {
    return isCompleted_;
}

const std::vector<SessionQuestionRecord>& InterviewSession::getRecords() const noexcept {
    return records_;
}
