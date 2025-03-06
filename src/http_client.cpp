//
// Created by Darryl West on 3/5/25.
//

#include <spdlog/spdlog.h>

#include <app/http_client.hpp>
#include <app/types.hpp>
#include <thread>

namespace soxlib {
    HttpResponse::HttpResponse(const httplib::Result& result) {
        if (result) {
            status = result->status;
            body = result->body;

            // TODO to compensate for multi-map
            // headers = result->headers;

            // TODO remove when multi-map is installed
            headers.clear();
            for (const auto& [key, value] : result->headers) {
                headers[key] = value;
            }

            if (headers.size() != result->headers.size()) {
                spdlog::error("missing headers from map (multi-map not installed!)");
            }

        } else {
            // TODO fix this to set the body to httplib::to_string(err)
            auto err = httplib::to_string(result.error());

            spdlog::error("result failed: {}", err);
            status = 500;
            body = "Request failed";
        }
    }

    HttpResponse::HttpResponse(int s, Str b, Headers h)
        : status(s), body(std::move(b)), headers(std::move(h)) {}

    HttpClient::HttpClient(Str url, Optional<Str> api_key)
        : base_url(std::move(url)), key(std::move(api_key)), cli(base_url) {
        if (api_key) {
            cli.set_default_headers(
                {{"Authorization", "Bearer " + *key}, {"Content-Type", "application/json"}});
        }

        cli.set_connection_timeout(10);
        cli.set_read_timeout(10);
    }

    HttpResponse HttpClient::Get(const std::string& path) {
        return WithRetry([&]() {
            LogRequest("GET", path);
            auto result = cli.Get(path.c_str());
            LogResponse(result);
            return HttpResponse(result);
        });
    }

    HttpResponse HttpClient::Post(const Str& path, const Str& body, const Str& contentType) {
        return WithRetry([&]() {
            LogRequest("POST", path);
            auto result = cli.Post(path.c_str(), body, contentType);
            LogResponse(result);
            return HttpResponse(result);
        });
    }

    HttpResponse HttpClient::Put(const Str& path, const Str& body, const Str& contentType) {
        return WithRetry([&]() {
            LogRequest("PUT", path);
            auto result = cli.Put(path.c_str(), body, contentType);
            LogResponse(result);
            return HttpResponse(result);
        });
    }

    HttpResponse HttpClient::Delete(const Str& path) {
        return WithRetry([&]() {
            LogRequest("DELETE", path);
            auto result = cli.Delete(path.c_str());
            LogResponse(result);
            return HttpResponse(result);
        });
    }

    template <typename F> HttpResponse HttpClient::WithRetry(F&& func, int maxRetries) {
        for (int i = 0; i < maxRetries; ++i) {
            try {
                return func();
            } catch (const std::exception& e) {
                if (i == maxRetries - 1) throw;
                std::this_thread::sleep_for(std::chrono::seconds(1 << i));
            }
        }
        throw std::runtime_error("Max retries exceeded");
    }

    void HttpClient::LogRequest(const Str& method, const Str& path) {
        // Add your logging logic here
        spdlog::info("method: {}, path: {}", method, path);
    }

    void HttpClient::LogResponse(const httplib::Result& result) {
        // Add your logging logic here
        spdlog::debug("Response: {}", result->body);
    }
}  // namespace soxlib
