#pragma once

#include <string>
#include <vector>

// OOP Concept: Encapsulation
// Encapsulates core user identity, credentials, academic affiliation, and streak telemetry.
class User {
private:
    std::string id_;
    std::string name_;
    std::string email_;
    std::string passwordHash_; // Encapsulated: never exposed in plain text
    std::string salt_;
    std::string role_; // "student" or "admin"
    std::string course_;
    std::string college_;
    std::string branch_;
    int streak_{0};
    int longestStreak_{0};
    std::string lastActiveDate_;
    std::vector<std::string> activityDates_;
    std::vector<std::string> bookmarks_;

public:
    User(std::string id,
         std::string name,
         std::string email,
         std::string passwordHash,
         std::string salt,
         std::string role = "student",
         std::string course = "CSE",
         std::string college = "Unspecified",
         std::string branch = "Unspecified",
         int streak = 0,
         int longestStreak = 0,
         std::string lastActiveDate = "");

    virtual ~User() = default;

    // Getters and setters enforcing encapsulation
    const std::string& getId() const noexcept;
    const std::string& getName() const noexcept;
    const std::string& getEmail() const noexcept;
    const std::string& getPasswordHash() const noexcept;
    const std::string& getSalt() const noexcept;
    const std::string& getRole() const noexcept;
    const std::string& getCourse() const noexcept;
    const std::string& getCollege() const noexcept;
    const std::string& getBranch() const noexcept;
    int getStreak() const noexcept;
    int getLongestStreak() const noexcept;
    const std::string& getLastActiveDate() const noexcept;
    const std::vector<std::string>& getActivityDates() const noexcept;
    const std::vector<std::string>& getBookmarks() const noexcept;

    void setName(const std::string& name);
    void setEmail(const std::string& email);
    void setPasswordHash(const std::string& hash);
    void setSalt(const std::string& salt);
    void setCourse(const std::string& course);
    void setCollege(const std::string& college);
    void setBranch(const std::string& branch);
    void setStreak(int streak);
    void setLongestStreak(int longest);
    void setLastActiveDate(const std::string& date);
    void setActivityDates(std::vector<std::string> dates);
    void setBookmarks(std::vector<std::string> bookmarks);

    void addActivityDate(const std::string& date);
    void incrementStreak();
    void resetStreak();

    void addBookmark(const std::string& questionId);
    void removeBookmark(const std::string& questionId);
    bool hasBookmark(const std::string& questionId) const;

    virtual bool isAdmin() const noexcept;
};
