#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() function
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include <app/version.hpp>
#include <app/types.hpp>
#include <vendor/ansi_colors.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

TEST_CASE("Version test", "[version]") {
    using namespace colors;
    spdlog::set_level(spdlog::level::off); // Setup: Disable logging

    constexpr auto vers = app::Version();
    const Str ss = vers.to_string();
    fmt::print("App Version: {}{}{}\n", cyan, ss, reset);

    INFO("Checking Application Version: " << vers.to_string());

    REQUIRE(vers.major == 0);
    REQUIRE(vers.minor == 6);
    REQUIRE(vers.patch == 5);
    REQUIRE(vers.build > 200);

    REQUIRE(ss.starts_with("0.6."));
}
