#include "ai/ResponseCache.h"
#include "auth/Sha256.h"

std::string ResponseCache::hashRequest(const LLMRequest& request) {
    std::string composite = request.systemPrompt + "|||" + request.userMessage;
    return Sha256::hash(composite);
}

std::optional<LLMResponse> ResponseCache::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = cache_.find(key);
    if (it != cache_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void ResponseCache::put(const std::string& key, const LLMResponse& response) {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_[key] = response;
}

void ResponseCache::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
}

size_t ResponseCache::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return cache_.size();
}
