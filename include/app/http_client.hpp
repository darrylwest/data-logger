//
// Created by Darryl West on 3/5/25.
//

#pragma once

#include <vendor/httplib.h>
#include <memory>
#include <functional>
#include <string>
#include <optional>
#include <unordered_map>

// Response wrapper
class HttpResponse {
public:
    int status;
    std::string body;
    std::unordered_map<std::string, std::string> headers;

    explicit HttpResponse(const httplib::Result& result);
    HttpResponse(int s, std::string b,
                std::unordered_map<std::string, std::string> h = {});
};

// Interface for HTTP client
class IHttpClient {
public:
    virtual ~IHttpClient() = default;
    virtual HttpResponse Get(const std::string& path) = 0;
    virtual HttpResponse Post(const std::string& path,
                            const std::string& body,
                            const std::string& contentType) = 0;
    // Add other methods as needed
};

// Production implementation
class HttpClient : public IHttpClient {
public:
    explicit HttpClient(std::string baseUrl,
                       std::optional<std::string> apiKey = std::nullopt);

    HttpResponse Get(const std::string& path) override;
    HttpResponse Post(const std::string& path,
                     const std::string& body,
                     const std::string& contentType) override;

private:
    std::string baseUrl_;
    std::optional<std::string> apiKey_;
    std::unique_ptr<httplib::Client> client_;

    template<typename F>
    HttpResponse WithRetry(F&& func, int maxRetries = 3);
    void LogRequest(const std::string& method, const std::string& path);
    void LogResponse(const httplib::Result& result);
};

// Mock implementation for testing
class MockHttpClient : public IHttpClient {
public:
    using ResponseHandler = std::function<HttpResponse(const std::string&)>;

    void SetGetHandler(ResponseHandler handler);
    void SetPostHandler(ResponseHandler handler);

    HttpResponse Get(const std::string& path) override;
    HttpResponse Post(const std::string& path,
                     const std::string& body,
                     const std::string& contentType) override;

private:
    ResponseHandler getHandler_;
    ResponseHandler postHandler_;
};