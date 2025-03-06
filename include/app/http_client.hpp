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

    constexpr time_t TIMEOUT_MILLIS = 6000;
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
            constexpr auto timeout = std::chrono::milliseconds{TIMEOUT_MILLIS};
            cli.set_connection_timeout(timeout);
            cli.set_read_timeout(timeout);
            cli.set_write_timeout(timeout);

            return cli;
        }
    };
}  // namespace soxlib
