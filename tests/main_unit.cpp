#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() function

#include <vendor/ansi_colors.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

struct MainTestSetup {
    MainTestSetup() {
        using namespace colors;
        fmt::print("{}Catch2 unit test setup.{}\n", green, reset);
        spdlog::set_level(spdlog::level::off); // Setup: Disable logging
    }

    ~MainTestSetup() {
        // using namespace colors;
        // fmt::print("{}unit test tear-down.{}\n", green, reset);
        // app::cfgsvc::stop_worker();
    }
};
