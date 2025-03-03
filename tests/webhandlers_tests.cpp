//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/types.hpp>
#include <app/webhandlers.hpp>

using namespace app;

struct WebHandlersTestSetup {
    WebHandlersTestSetup() {
        spdlog::set_level(spdlog::level::info);
    }

    ~WebHandlersTestSetup() {
        spdlog::set_level(spdlog::level::off);
    }
};

TEST_CASE("WebHandlers Tests", "[webhandlers][chart_data]") {
    // TODO implement
    const Str ss = "test string";
    REQUIRE(ss == "test string");
}

TEST_CASE("WebHandlers Tests", "[webhandlers][temps_response]") {
    // TODO implement
    const Str ss = "test string";
    REQUIRE(ss == "test string");
}
