#pragma once

#include "ai/ILLMClient.h"
#include <queue>
#include <vector>

// OOP Concept: Test Double (Mock Object Pattern)
// Simulates LLM responses for isolated, network-independent unit tests.
class MockLLMClient : public ILLMClient {
private:
    std::string defaultResponse_;
    std::queue<LLMResponse> scriptedResponses_;
    std::vector<LLMRequest> recordedRequests_;
    bool simulateError_{false};
    std::string simulateErrorMessage_{"Simulated LLM network failure"};

public:
    explicit MockLLMClient(std::string defaultResponse = "{}");
    ~MockLLMClient() override = default;

    LLMResponse complete(const LLMRequest& request) override;
    std::string getProviderName() const override;

    void enqueueResponse(const LLMResponse& response);
    void setDefaultResponse(std::string response);
    void setSimulateError(bool simulate, std::string message = "Simulated error");

    const std::vector<LLMRequest>& getRecordedRequests() const noexcept { return recordedRequests_; }
    void clearRecordedRequests() noexcept { recordedRequests_.clear(); }
};
