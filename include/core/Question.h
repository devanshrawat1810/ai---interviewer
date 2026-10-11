#pragma once

#include <string>
#include <vector>
#include "core/Difficulty.h"

// OOP Concept: Abstraction
// Defines the abstract base contract for all interview questions.
// Client code interacts with questions polymorphically without knowing the concrete subtype.
class Question {
protected:
    std::string id_;
    std::string subject_;
    std::string topic_;
    Difficulty difficulty_;
    std::string type_;
    std::string text_;
    std::string explanation_;
    bool isActive_{true}; // Soft delete flag to preserve historical attempt references

public:
    Question(std::string id,
             std::string subject,
             std::string topic,
             Difficulty difficulty,
             std::string type,
             std::string text,
             std::string explanation,
             bool isActive = true);

    virtual ~Question() = default;

    virtual void display() const = 0;
    virtual bool evaluate(const std::string& userResponse) const = 0;

    const std::string& getId() const noexcept;
    const std::string& getSubject() const noexcept;
    const std::string& getTopic() const noexcept;
    Difficulty getDifficulty() const noexcept;
    const std::string& getType() const noexcept;
    const std::string& getText() const noexcept;
    const std::string& getExplanation() const noexcept;
    bool isActive() const noexcept;

    void setActive(bool active) noexcept;
    void setText(std::string text);
    void setExplanation(std::string explanation);
};
