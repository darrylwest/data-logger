#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() function

#include <vendor/ansi_colors.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include "test_helpers.hpp"

// start the configuration service and the timer
void start_config_service() {
    using namespace app::cfgsvc;

    ServiceContext ctx;
    ctx.json_text = helpers::full_config_json_text;
    ctx.sleep_duration = std::chrono::seconds(0);
    configure(ctx);
}

struct MainTestSetup {
    MainTestSetup() {
        using namespace colors;
        fmt::print("{}Catch2 unit test setup.{}\n", green, reset);
        spdlog::set_level(spdlog::level::off); // Setup: Disable logging
        start_config_service();
    }

    ~MainTestSetup() {
        // using namespace colors;
        // fmt::print("{}unit test tear-down.{}\n", green, reset);
        // app::cfgsvc::stop_worker();
    }
};

const MainTestSetup setup = MainTestSetup();
