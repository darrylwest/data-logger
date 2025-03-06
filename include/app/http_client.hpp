//
// Created by Darryl West on 3/5/25.
//

#pragma once

#include <vendor/httplib.h>

#include <app/types.hpp>
#include <functional>
#include <optional>
#include <string>

namespace soxlib {

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

    using ResponseHandler = std::function<HttpResponse(const Str& body, const int status_code)>;

    class HttpClient {
      public:
        explicit HttpClient(Str url, Optional<Str> api_key = std::nullopt);

        HttpResponse Get(const Str& path);
        HttpResponse Post(const Str& path, const Str& body, const Str& contentType);
        HttpResponse Put(const Str& path, const Str& body, const Str& content_type);
        HttpResponse Delete(const Str& path);

      private:
        Str base_url;
        Optional<std::string> key;
        std::unique_ptr<httplib::Client> cli;
        // httplib::Client cli;

        template <typename F> HttpResponse WithRetry(F&& func, int maxRetries = 3);
        void LogRequest(const std::string& method, const std::string& path);
        void LogResponse(const httplib::Result& result);

        ResponseHandler handler;
    };
}  // namespace soxlib
