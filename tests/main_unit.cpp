#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() function
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include <app/version.hpp>
#include <app/types.hpp>
#include <vendor/ansi_colors.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

void start_config_service() {
    using namespace app::cfgsvc;

    // start the config service
    ServiceContext ctx;
    ctx.sleep_duration = std::chrono::seconds(1);
    configure(ctx);
}

struct MainTestSetup {
    MainTestSetup() {
        using namespace colors;
        fmt::print("{}Catch2 Unit Test:{}\n", cyan, reset);
        spdlog::set_level(spdlog::level::off); // Setup: Disable logging
        start_config_service();
    }
};

TEST_CASE("Version test", "[version]") {
    spdlog::set_level(spdlog::level::off); // Setup: Disable logging
    MainTestSetup setup;

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
