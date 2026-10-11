#pragma once

#include "ai/ILLMClient.h"
#include <map>
#include <string>
#include <mutex>
#include <optional>

// OOP Concept: Caching & Performance Optimization
// Deduplicates identical generative queries within an interview session using prompt SHA-256 hashes.
class ResponseCache {
private:
    mutable std::mutex mutex_;
    std::map<std::string, LLMResponse> cache_;

public:
    ResponseCache() = default;

    static std::string hashRequest(const LLMRequest& request);

    std::optional<LLMResponse> get(const std::string& key) const;
    void put(const std::string& key, const LLMResponse& response);
    void clear();
    size_t size() const;
};
