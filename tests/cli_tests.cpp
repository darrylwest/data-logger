//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/types.hpp>
#include <spdlog/spdlog.h>
#include <app/cli.hpp>

struct CLITestSetup {
    CLITestSetup() {
        spdlog::set_level(spdlog::level::info); // Setup: Disable logging
    }

    ~CLITestSetup() {
        spdlog::set_level(spdlog::level::off); // Teardown: Restore logging
    }
};

TEST_CASE_METHOD(CLITestSetup, "CLI Tests", "[cli]") {
    const Str ss = "test string";
    REQUIRE(ss == "test string");
    spdlog::info("cli tests");
}
