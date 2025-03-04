//
// 2025-02-07 15:47:16 dpw
//

#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/types.hpp>
#include <app/version.hpp>

// special
#include <app/cfgsvc.hpp>
#include <app/client.hpp>
#include <app/exceptions.hpp>
#include <app/nodes.hpp>
#include <thread>

using namespace app::taskrunner;

void start_config_service() {
    using namespace app;

    cfgsvc::ServiceContext ctx;
    ctx.sleep_duration = std::chrono::seconds(5);

    cfgsvc::configure(ctx);
}

int main(int argc, char* argv[]) {
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");

    // configuration service has to work or we are toast
    try {
        start_config_service();
    } catch (std::exception& e) {
        std::cerr << "Configuration died on startup, " << e.what() << ", aborting..." << std::endl;
        std::exit(1);
    }

    const auto vers = app::Version();

    const auto shutdown = [](int code) {
        // spdlog::info("Shutdown code: {}", code);
        // better to send a kill statement
        exit(code);
    };

    const auto jconf = app::cfgsvc::webservice();
    const auto webconfig = app::config::webconfig_from_json(jconf);

    const auto params = app::config::CliParams{
        .argc = argc, .argv = argv, .config = webconfig, .shutdown = shutdown};
    auto config = app::config::parse_cli(params);
    spdlog::info("DataLogger Version: {}", vers.to_string());

    // implement a global shutdown flag to be read by threads
    // clear it here, the set it when run service exits
    // or figure out how to use std::terminate with hooks

    // now start the web/http listener
    auto ok = app::run_service(config);
    spdlog::info("Server shutdown, code: {}.", ok);

    // ok, not clean but it works;
    // std::terminate();

    return 0;
}
