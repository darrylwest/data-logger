//
// Created by Darryl West on 3/1/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/version.hpp>
#include <vendor/ansi_colors.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

TEST_CASE("Version test", "[version]") {
    spdlog::set_level(spdlog::level::off); // Setup: Disable logging

    using namespace colors;

    constexpr auto vers = app::Version();
    const Str ss = vers.to_string();
    fmt::print("Unit Test App Version: {}{}{}\n", cyan, ss, reset);

    INFO("Checking Application Version: " << vers.to_string());

    REQUIRE(vers.major == 0);
    REQUIRE(vers.minor == 6);
    REQUIRE(vers.patch == 5);
    REQUIRE(vers.build > 200);

    REQUIRE(ss.starts_with("0.6."));
}
