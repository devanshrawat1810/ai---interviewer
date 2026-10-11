#pragma once

#include <string>
#include <vector>
#include <utility>

struct HttpResponse {
    int statusCode{0};
    std::string body;
};

// OOP Concept: Dependency Inversion & Strategy Pattern
// Abstract transport interface enabling interchangeable HTTP transports (libcurl, test mocks).
class IHttpTransport {
public:
    virtual ~IHttpTransport() = default;

    virtual HttpResponse executePost(const std::string& url,
                                     const std::vector<std::string>& headers,
                                     const std::string& jsonBody,
                                     int timeoutSeconds) = 0;
};
