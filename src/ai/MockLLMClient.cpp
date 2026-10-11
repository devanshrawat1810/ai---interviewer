#include "ai/MockLLMClient.h"

// OOP Concept: Test Double Implementation
// Provides scripted and deterministic responses for test suites.

MockLLMClient::MockLLMClient(std::string defaultResponse)
    : defaultResponse_(std::move(defaultResponse)) {}

LLMResponse MockLLMClient::complete(const LLMRequest& request) {
    recordedRequests_.push_back(request);

    if (simulateError_) {
        LLMResponse err;
        err.success = false;
        err.errorMessage = simulateErrorMessage_;
        return err;
    }

    if (!scriptedResponses_.empty()) {
        LLMResponse nextResp = scriptedResponses_.front();
        scriptedResponses_.pop();
        return nextResp;
    }

    LLMResponse resp;
    resp.success = true;
    resp.content = defaultResponse_;
    resp.promptTokens = static_cast<int>(request.userMessage.size() / 4);
    resp.completionTokens = static_cast<int>(defaultResponse_.size() / 4);
    return resp;
}

std::string MockLLMClient::getProviderName() const {
    return "MockLLMClient (Test)";
}

void MockLLMClient::enqueueResponse(const LLMResponse& response) {
    scriptedResponses_.push(response);
}

void MockLLMClient::setDefaultResponse(std::string response) {
    defaultResponse_ = std::move(response);
}

void MockLLMClient::setSimulateError(bool simulate, std::string message) {
    simulateError_ = simulate;
    simulateErrorMessage_ = std::move(message);
}
