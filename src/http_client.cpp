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
            status = 500;
            body = to_string(result.error());
        }
    }

    HttpResponse::HttpResponse(int s, Str b, Headers h)
        : status(s), body(std::move(b)), headers(std::move(h)) {}

    HttpClient::HttpClient(Str url, Optional<Str> api_key)
        : base_url(std::move(url)), auth_key(std::move(api_key)) {}

    HttpResponse HttpClient::Get(const Str& path) {
        LogRequest("GET", path);

        if (mock_response) {
            spdlog::info("-------------- mock get {} ", mock_response->body);
            return *mock_response;
        }

        httplib::Client cli(base_url);
        auto result = cli.Get(path.c_str());
        LogResponse(result);

        return HttpResponse(result);
    }

    HttpResponse HttpClient::Post(const Str& path, const Str& body, const Str& contentType) {
        LogRequest("POST", path);

        if (mock_response) {
            spdlog::info("-------------- mock post {} ", mock_response->body);
            return *mock_response;
        }

        auto cli = create_client();
        auto result = cli.Post(path.c_str(), body, contentType);
        LogResponse(result);
        return HttpResponse(result);
    }

    HttpResponse HttpClient::Put(const Str& path, const Str& body, const Str& contentType) {
        LogRequest("PUT", path);

        if (mock_response) {
            spdlog::info("-------------- mock put {} ", mock_response->body);
            return *mock_response;
        }

        auto cli = create_client();
        auto result = cli.Put(path.c_str(), body, contentType);
        LogResponse(result);
        return HttpResponse(result);
    }

    HttpResponse HttpClient::Delete(const Str& path) {
        LogRequest("DELETE", path);

        if (mock_response) {
            spdlog::info("-------------- mock delete {} ", mock_response->body);
            return *mock_response;
        }

        auto cli = create_client();
        auto result = cli.Delete(path.c_str());
        LogResponse(result);
        return HttpResponse(result);
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
        spdlog::info("request: {} : {}", method, path);
    }

    void HttpClient::LogResponse(const httplib::Result& result) {
        // Add your logging logic here
        if (result) {
            spdlog::debug("Response: {}", result->body);
        } else {
            auto e = result.error();
            spdlog::error("Response: {}", httplib::to_string(e));
        }
    }

    void HttpClient::set_handler(const HttpResponse& response_handler) {
        mock_response = response_handler;
    }
}  // namespace soxlib
