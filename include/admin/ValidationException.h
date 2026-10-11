#pragma once

#include <stdexcept>
#include <string>

// OOP Concept: Exception Hierarchy & Inheritance
// Exception thrown when question schema or domain data fails validation rules.
class ValidationException : public std::invalid_argument {
public:
    explicit ValidationException(const std::string& message)
        : std::invalid_argument("Validation Error: " + message) {}
};
