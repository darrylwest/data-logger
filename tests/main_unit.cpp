#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() function

#include <print>
#include <datetimelib/perftimer.hpp>
#include <vendor/ansi_colors.hpp>
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

PerfTimer perf_timer("Catch2 Unit Tests");

struct MainTestSetup {

    MainTestSetup() {
        using namespace colors;
        std::println("{}Catch2 unit test setup.{}", green, reset);
        spdlog::set_level(spdlog::level::critical); // Setup: Disable logging
        start_config_service();
        perf_timer.start();
    }

    ~MainTestSetup() {
        using namespace colors;
        perf_timer.stop();
        std::print("{}", bright::yellow);
        perf_timer.show_duration();
        std::print("{}", reset);
    }
};

const MainTestSetup setup = MainTestSetup();
