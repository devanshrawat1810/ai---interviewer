#include "gui/ErrorTranslator.h"
#include "db/DatabaseExceptions.h"
#include "ai/AiExceptions.h"
#include "admin/ValidationException.h"

std::string ErrorTranslator::translate(const std::exception& e) {
    // Check specific exception types first
    if (dynamic_cast<const ConnectionException*>(&e)) {
        return "Unable to connect to the database. The system is operating in local offline mode.";
    }
    if (dynamic_cast<const DuplicateKeyException*>(&e)) {
        return "An account with this email address already exists. Please log in or use a different email.";
    }
    if (dynamic_cast<const RateLimitException*>(&e)) {
        return "AI request limit reached. Falling back to standard evaluation. Please try again later.";
    }
    if (dynamic_cast<const TimeoutException*>(&e)) {
        return "The AI request took too long to respond. Standard evaluation has been used instead.";
    }
    if (dynamic_cast<const ValidationException*>(&e)) {
        return std::string("Please check your entry: ") + e.what();
    }
    return translate(std::string(e.what()));
}

std::string ErrorTranslator::translate(const std::string& rawError) {
    if (rawError.empty()) {
        return "An unexpected error occurred. Please try again.";
    }
    if (rawError.find("lockout") != std::string::npos || rawError.find("locked") != std::string::npos) {
        return "Account temporarily locked due to 5 failed attempts. Please wait 15 minutes before retrying.";
    }
    if (rawError.find("password") != std::string::npos || rawError.find("credentials") != std::string::npos) {
        return "Incorrect email or password. Please check your credentials and try again.";
    }
    if (rawError.find("not found") != std::string::npos) {
        return "The requested item could not be found. It may have been moved or updated.";
    }
    if (rawError.find("6000") != std::string::npos || rawError.find("character") != std::string::npos) {
        return "Resume exceeds the 6,000-character limit. Please shorten your resume text and try again.";
    }
    return rawError;
}
