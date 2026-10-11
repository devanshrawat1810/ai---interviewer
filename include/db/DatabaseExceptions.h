#pragma once

#include <stdexcept>
#include <string>

// OOP Concept: Exception Hierarchy & Inheritance
// Custom domain exception hierarchy for database operations.
class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& message)
        : std::runtime_error(message) {}
};

class ConnectionException : public DatabaseException {
public:
    explicit ConnectionException(const std::string& message)
        : DatabaseException("Database Connection Error: " + message) {}
};

class QueryException : public DatabaseException {
public:
    explicit QueryException(const std::string& message)
        : DatabaseException("Database Query Error: " + message) {}
};

class DuplicateKeyException : public DatabaseException {
public:
    explicit DuplicateKeyException(const std::string& message)
        : DatabaseException("Duplicate Key Conflict: " + message) {}
};
