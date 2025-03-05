//
// Created by Darryl West on 3/5/25.
//

#pragma once

#include <vendor/httplib.h>

#include <app/types.hpp>
#include <functional>
#include <optional>
#include <string>

// using Headers = std::unordered_multimap<std::string, std::string>;
//, detail::case_ignore::hash, detail::case_ignore::equal_to>;

using Headers = std::unordered_map<Str, Str>;

// Response wrapper
class HttpResponse {
  public:
    int status;
    Str body;
    Headers headers;

    explicit HttpResponse(const httplib::Result& result);
    HttpResponse(int s, Str b, Headers h = {});
};

// Interface for HTTP client
class IHttpClient {
  public:
    virtual ~IHttpClient() = default;
    virtual HttpResponse Get(const Str& path) = 0;
    virtual HttpResponse Post(const Str& path, const Str& body, const Str& contentType) = 0;
    virtual HttpResponse Put(const Str& path, const Str& body, const Str& contentType) = 0;
    virtual HttpResponse Delete(const Str& path) = 0;
    // Add other methods as needed
};

// Production implementation
class HttpClient : public IHttpClient {
  public:
    explicit HttpClient(Str baseUrl, std::optional<Str> apiKey = std::nullopt);

    HttpResponse Get(const Str& path) override;
    HttpResponse Post(const Str& path, const Str& body, const Str& contentType) override;
    // HttpResponse Put(const Str& path, const Str& body) override;
    HttpResponse Put(const std::string& path, const std::string& body,
                     const std::string& content_type) override;
    HttpResponse Delete(const std::string& path) override;

  private:
    std::string baseUrl_;
    std::optional<std::string> apiKey_;
    std::unique_ptr<httplib::Client> client_;

    template <typename F> HttpResponse WithRetry(F&& func, int maxRetries = 3);
    void LogRequest(const std::string& method, const std::string& path);
    void LogResponse(const httplib::Result& result);
};

// Mock implementation for testing
class MockHttpClient : public IHttpClient {
  public:
    using ResponseHandler = std::function<HttpResponse(const Str&)>;

    void SetGetHandler(ResponseHandler handler);
    void SetPostHandler(ResponseHandler handler);

    HttpResponse Get(const Str& path) override;
    HttpResponse Post(const Str& path, const Str& body, const Str& contentType) override;
    HttpResponse Put(const Str& path, const Str& body, const Str& contentType) override;
    HttpResponse Delete(const Str& path) override;

  private:
    ResponseHandler getHandler_;
    ResponseHandler postHandler_;
};