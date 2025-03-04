//
// Created by Darryl West on 3/3/25.
//

#pragma once

#include <vendor/httplib.h>
#include <spdlog/spdlog.h>
#include <app/types.hpp>


class MockHttpClient : public httplib::Client {
public:
    explicit MockHttpClient(const Str& host) : Client(host) {}

    // GET
    Result Get(const char* path) {
        spdlog::debug("get: {}", path);
        return std::move(expected_response);
    }

    // POST
    Result Post(const char* path, const std::string& body, const char*) {
        spdlog::debug("Post: {} {}", path, body);
        return std::move(expected_response);
    }

    // PUT
    Result Put(const char* path, const std::string& body, const char*) {
        spdlog::debug("Put: {} {}", path, body);
        return std::move(expected_response);
    }

    // DELETE
    Result Delete(const char* path) {
        spdlog::debug("Delete {}", path);
        return std::move(expected_response);
    }

    void set_expected_response(const Str& body, const int status) {
        expected_response->body = body;
        expected_response->status = status;
    }
private:
    Result expected_response;
};

