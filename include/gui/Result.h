#pragma once

#include <string>
#include <utility>

// OOP Concept: Generic Result Pattern (Encapsulation)
// Strongly-typed return payload for asynchronous workers carrying either a value or user-facing error.
template <typename T>
class Result {
private:
    bool success_{false};
    T value_{};
    std::string errorMessage_{};

public:
    Result() = default;

    static Result<T> ok(T value) {
        Result<T> r;
        r.success_ = true;
        r.value_ = std::move(value);
        return r;
    }

    static Result<T> error(std::string msg) {
        Result<T> r;
        r.success_ = false;
        r.errorMessage_ = std::move(msg);
        return r;
    }

    bool isOk() const noexcept { return success_; }
    bool isError() const noexcept { return !success_; }

    const T& value() const { return value_; }
    T& value() { return value_; }

    const std::string& error() const noexcept { return errorMessage_; }
};

// Void specialization for operations with no return payload
template <>
class Result<void> {
private:
    bool success_{false};
    std::string errorMessage_{};

public:
    Result() = default;

    static Result<void> ok() {
        Result<void> r;
        r.success_ = true;
        return r;
    }

    static Result<void> error(std::string msg) {
        Result<void> r;
        r.success_ = false;
        r.errorMessage_ = std::move(msg);
        return r;
    }

    bool isOk() const noexcept { return success_; }
    bool isError() const noexcept { return !success_; }
    const std::string& error() const noexcept { return errorMessage_; }
};
