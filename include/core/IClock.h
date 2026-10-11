#pragma once

#include <string>

// OOP Concept: Dependency Inversion Principle & Strategy
// Abstract clock interface decoupling time/date querying from the operating system.
class IClock {
public:
    virtual ~IClock() = default;

    // Returns current date in YYYY-MM-DD format
    virtual std::string getCurrentDate() const = 0;
};
