#pragma once

#include <string>

// OOP Concept: Composition Component
// Represents the fundamental atomic unit of learning or evaluation within a subject.
class Topic {
private:
    std::string id_;
    std::string name_;
    std::string description_;

public:
    Topic(std::string id, std::string name, std::string description = "");

    const std::string& getId() const noexcept;
    const std::string& getName() const noexcept;
    const std::string& getDescription() const noexcept;
};
