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
    // once an hour
    ctx.sleep_duration = std::chrono::seconds(3600);

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

    auto config = app::config::parse_cli(argc, argv);
    spdlog::info("DataLogger Version: {}", vers.to_string());

    // implement a global shutdown flag to be read by threads
    // clear it here, the set it when run service exits
    // or figure out how to use std::terminate with hooks

    // now start the web/http listener
    auto ok = app::run_service(config);
    spdlog::info("Server shutdown, code: {}.", ok);

    // ok, not clean but it works;
    std::terminate();

    return 0;
}
