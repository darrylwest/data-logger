//
// 2025-02-07 15:47:16 dpw
//

// #include "precompiled.hpp"
#include <spdlog/spdlog.h>

#include <app/cfgsvc.hpp>
#include <app/cli.hpp>
#include <app/nodes.hpp>
#include <app/service.hpp>
#include <app/version.hpp>

#include "precompiled.hpp"

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
        spdlog::error("Configuration died on startup, {}, aborting...", e.what());
        std::exit(1);
    }

    const auto vers = app::Version();

    const auto shutdown = [](int code) {
        // spdlog::info("Shutdown code: {}", code);
        // better to send a kill statement?
        exit(code);
    };

    const auto jconf = app::cfgsvc::webservice();
    const auto webconfig = app::config::webconfig_from_json(jconf);

    const auto params = app::config::CliParams{
        .argc = argc, .argv = argv, .config = webconfig, .shutdown = shutdown};

    auto config = app::config::parse_cli(params);
    spdlog::info("DataLogger Version: {}", vers.to_string());

    // now start the web/http listener
    auto ok = app::service::run_service(config);
    spdlog::info("Server shutdown, code: {}.", ok);

    return 0;
}
