#include "core/User.h"
#include <utility>
#include <algorithm>

// OOP Concept: Encapsulation
// Implementations of encapsulated User getters and mutating methods.

User::User(std::string id,
           std::string name,
           std::string email,
           std::string passwordHash,
           std::string salt,
           std::string role,
           std::string course,
           std::string college,
           std::string branch,
           int streak,
           int longestStreak,
           std::string lastActiveDate)
    : id_(std::move(id)),
      name_(std::move(name)),
      email_(std::move(email)),
      passwordHash_(std::move(passwordHash)),
      salt_(std::move(salt)),
      role_(std::move(role)),
      course_(std::move(course)),
      college_(std::move(college)),
      branch_(std::move(branch)),
      streak_(streak),
      longestStreak_(longestStreak),
      lastActiveDate_(std::move(lastActiveDate)) {
    if (longestStreak_ < streak_) {
        longestStreak_ = streak_;
    }
}

const std::string& User::getId() const noexcept { return id_; }
const std::string& User::getName() const noexcept { return name_; }
const std::string& User::getEmail() const noexcept { return email_; }
const std::string& User::getPasswordHash() const noexcept { return passwordHash_; }
const std::string& User::getSalt() const noexcept { return salt_; }
const std::string& User::getRole() const noexcept { return role_; }
const std::string& User::getCourse() const noexcept { return course_; }
const std::string& User::getCollege() const noexcept { return college_; }
const std::string& User::getBranch() const noexcept { return branch_; }
int User::getStreak() const noexcept { return streak_; }
int User::getLongestStreak() const noexcept { return longestStreak_; }
const std::string& User::getLastActiveDate() const noexcept { return lastActiveDate_; }
const std::vector<std::string>& User::getActivityDates() const noexcept { return activityDates_; }
const std::vector<std::string>& User::getBookmarks() const noexcept { return bookmarks_; }

void User::setName(const std::string& name) { name_ = name; }
void User::setEmail(const std::string& email) { email_ = email; }
void User::setPasswordHash(const std::string& hash) { passwordHash_ = hash; }
void User::setSalt(const std::string& salt) { salt_ = salt; }
void User::setCourse(const std::string& course) { course_ = course; }
void User::setCollege(const std::string& college) { college_ = college; }
void User::setBranch(const std::string& branch) { branch_ = branch; }
void User::setStreak(int streak) {
    streak_ = streak;
    if (streak_ > longestStreak_) longestStreak_ = streak_;
}
void User::setLongestStreak(int longest) { longestStreak_ = longest; }
void User::setLastActiveDate(const std::string& date) { lastActiveDate_ = date; }
void User::setActivityDates(std::vector<std::string> dates) { activityDates_ = std::move(dates); }
void User::setBookmarks(std::vector<std::string> bookmarks) { bookmarks_ = std::move(bookmarks); }

void User::addActivityDate(const std::string& date) {
    if (std::find(activityDates_.begin(), activityDates_.end(), date) == activityDates_.end()) {
        activityDates_.push_back(date);
    }
}

void User::incrementStreak() {
    ++streak_;
    if (streak_ > longestStreak_) {
        longestStreak_ = streak_;
    }
}

void User::resetStreak() {
    streak_ = 0;
}

void User::addBookmark(const std::string& questionId) {
    if (!hasBookmark(questionId)) {
        bookmarks_.push_back(questionId);
    }
}

void User::removeBookmark(const std::string& questionId) {
    auto it = std::find(bookmarks_.begin(), bookmarks_.end(), questionId);
    if (it != bookmarks_.end()) {
        bookmarks_.erase(it);
    }
}

bool User::hasBookmark(const std::string& questionId) const {
    return std::find(bookmarks_.begin(), bookmarks_.end(), questionId) != bookmarks_.end();
}

bool User::isAdmin() const noexcept {
    return role_ == "admin";
}
