#pragma once

#include <stdexcept>
#include <string>

// OOP Concept: Custom Exception Hierarchy
// Inherits std::runtime_error to provide specialized error categorization for AI and HTTP operations.
class AiException : public std::runtime_error {
public:
    explicit AiException(const std::string& message) : std::runtime_error(message) {}
};

class NetworkException : public AiException {
public:
    explicit NetworkException(const std::string& message) : AiException("Network Error: " + message) {}
};

class TimeoutException : public AiException {
public:
    explicit TimeoutException(const std::string& message) : AiException("Timeout Error: " + message) {}
};

class ApiException : public AiException {
private:
    int statusCode_{0};
public:
    ApiException(int statusCode, const std::string& message)
        : AiException("API Error (Status " + std::to_string(statusCode) + "): " + message),
          statusCode_(statusCode) {}

    int getStatusCode() const noexcept { return statusCode_; }
};

class RateLimitException : public ApiException {
public:
    explicit RateLimitException(const std::string& message)
        : ApiException(429, "Rate Limit Exceeded: " + message) {}
};

class ParseException : public AiException {
public:
    explicit ParseException(const std::string& message) : AiException("Parse Error: " + message) {}
};

class AiUnavailableException : public AiException {
public:
    explicit AiUnavailableException(const std::string& message) : AiException("AI Unavailable: " + message) {}
};
