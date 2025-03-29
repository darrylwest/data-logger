//
// Created by Darryl West on 3/5/25.
//
// http_client is a thin wrapper around httplib.  it provides a way to completely
// mock out the real server for unit tests.
//

#pragma once

#include <vendor/httplib.h>

#include <app/types.hpp>

#include "precompiled.hpp"

namespace soxlib {

    constexpr time_t CONNECT_TIMEOUT_SECONDS = 6;
    constexpr time_t READ_TIMEOUT_SECONDS = 4;
    constexpr time_t WRITE_TIMEOUT_SECONDS = 3;

    const Str APP_JSON = "application/json";

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

    // using ResponseHandler = Func<HttpResponse(const Str& body, int status_code)>;

    class HttpClient {
      public:
        explicit HttpClient(Str url, Optional<Str> api_key = std::nullopt);

        // get with specified path using base_url
        HttpResponse Get(const Str& path);

        // post with specified path using base_url and body and content type  defaults to
        // application/json
        HttpResponse Post(const Str& path, const Str& body, const Str& content_type = APP_JSON);

        // put with specified path using base_url and body;  defaults to application/json
        HttpResponse Put(const Str& path, const Str& body, const Str& content_type = APP_JSON);

        // get with specified path using base_url
        HttpResponse Delete(const Str& path);

        // use this method to switch between real and mock handlers
        void set_handler(const HttpResponse& response_handler);

      private:
        Str base_url;
        Optional<std::string> auth_key;

        template <typename F> HttpResponse WithRetry(F&& func, int maxRetries = 3);
        void LogRequest(const Str& method, const std::string& path);
        void LogResponse(const httplib::Result& result);

        // nothing here yet
        Optional<HttpResponse> mock_response;

        httplib::Client create_client() {
            httplib::Client cli(base_url);

            // set the timeouts
            cli.set_connection_timeout(CONNECT_TIMEOUT_SECONDS);
            cli.set_read_timeout(READ_TIMEOUT_SECONDS);
            cli.set_write_timeout(WRITE_TIMEOUT_SECONDS);

            constexpr auto max_timeout = std::chrono::milliseconds(10'000);
            cli.set_max_timeout(max_timeout);

            return cli;
        }
    };
}  // namespace soxlib
