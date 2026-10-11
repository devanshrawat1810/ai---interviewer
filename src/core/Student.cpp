#include "core/Student.h"
#include <algorithm>

// OOP Concept: Inheritance
// Implements Student metrics, progressive difficulty unlocks, and badge assignments.

Student::Student(std::string id,
                 std::string name,
                 std::string email,
                 std::string passwordHash,
                 std::string salt,
                 std::string course,
                 std::string college,
                 std::string branch,
                 int streak,
                 int longestStreak,
                 std::string lastActiveDate)
    : User(std::move(id),
           std::move(name),
           std::move(email),
           std::move(passwordHash),
           std::move(salt),
           "student",
           std::move(course),
           std::move(college),
           std::move(branch),
           streak,
           longestStreak,
           std::move(lastActiveDate)) {
    solvedByDifficulty_[Difficulty::EASY] = 0;
    solvedByDifficulty_[Difficulty::MEDIUM] = 0;
    solvedByDifficulty_[Difficulty::HARD] = 0;
}

void Student::recordChallengeSolve(const std::string& challengeId,
                                   const std::string& subject,
                                   Difficulty diff,
                                   int points) {
    if (solvedChallenges_.find(challengeId) == solvedChallenges_.end()) {
        solvedChallenges_.insert(challengeId);
        solvedByDifficulty_[diff]++;
        totalScore_ += points;

        int easySolved = solvedByDifficulty_[Difficulty::EASY];
        int medSolved = solvedByDifficulty_[Difficulty::MEDIUM];

        Difficulty currentMax = getUnlockedLevel(subject);
        if (currentMax == Difficulty::EASY && easySolved >= 3) {
            subjectUnlockedLevel_[subject] = Difficulty::MEDIUM;
        }
        if (subjectUnlockedLevel_[subject] == Difficulty::MEDIUM && medSolved >= 3) {
            subjectUnlockedLevel_[subject] = Difficulty::HARD;
        }
    }
}

void Student::recordQuestionAttempt(bool correct, int points) {
    totalQuestionsAttempted_++;
    if (correct) {
        totalQuestionsCorrect_++;
        totalScore_ += points;
    }
}

void Student::recordDailyChallengeCompletion(int bonusPoints) {
    dailyChallengesCompleted_++;
    totalScore_ += bonusPoints;
}

int Student::getSolvedCount(Difficulty diff) const {
    auto it = solvedByDifficulty_.find(diff);
    return (it != solvedByDifficulty_.end()) ? it->second : 0;
}

int Student::getTotalSolvedCount() const {
    return static_cast<int>(solvedChallenges_.size());
}

Difficulty Student::getUnlockedLevel(const std::string& subject) const {
    auto it = subjectUnlockedLevel_.find(subject);
    if (it != subjectUnlockedLevel_.end()) {
        return it->second;
    }
    return Difficulty::EASY;
}

bool Student::isLevelUnlocked(const std::string& subject, Difficulty diff) const {
    Difficulty unlocked = getUnlockedLevel(subject);
    if (diff == Difficulty::EASY) return true;
    if (diff == Difficulty::MEDIUM) return (unlocked == Difficulty::MEDIUM || unlocked == Difficulty::HARD);
    if (diff == Difficulty::HARD) return (unlocked == Difficulty::HARD);
    return false;
}

double Student::getAccuracy() const noexcept {
    if (totalQuestionsAttempted_ == 0) return 0.0;
    return (static_cast<double>(totalQuestionsCorrect_) / totalQuestionsAttempted_) * 100.0;
}

int Student::getTotalScore() const noexcept { return totalScore_; }
int Student::getDailyChallengesCompleted() const noexcept { return dailyChallengesCompleted_; }
const std::vector<std::string>& Student::getBadges() const noexcept { return badges_; }
const std::set<std::string>& Student::getSolvedChallenges() const noexcept { return solvedChallenges_; }

void Student::addBadge(const std::string& badgeName) {
    if (!hasBadge(badgeName)) {
        badges_.push_back(badgeName);
    }
}

bool Student::hasBadge(const std::string& badgeName) const {
    return std::find(badges_.begin(), badges_.end(), badgeName) != badges_.end();
}

void Student::setBadges(std::vector<std::string> badges) {
    badges_ = std::move(badges);
}

void Student::setSubjectUnlockedLevel(std::map<std::string, Difficulty> levels) {
    subjectUnlockedLevel_ = std::move(levels);
}

void Student::setSolvedChallenges(std::set<std::string> challenges) {
    solvedChallenges_ = std::move(challenges);
}

void Student::setSolvedCounts(int easy, int medium, int hard) {
    solvedByDifficulty_[Difficulty::EASY] = easy;
    solvedByDifficulty_[Difficulty::MEDIUM] = medium;
    solvedByDifficulty_[Difficulty::HARD] = hard;
}

void Student::setTotalQuestionsAttempted(int count) { totalQuestionsAttempted_ = count; }
void Student::setTotalQuestionsCorrect(int count) { totalQuestionsCorrect_ = count; }
void Student::setTotalScore(int score) { totalScore_ = score; }
void Student::setDailyChallengesCompleted(int count) { dailyChallengesCompleted_ = count; }
