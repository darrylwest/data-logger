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
#include <app/client.hpp>
#include <app/nodes.hpp>
#include <thread>

using namespace app::taskrunner;

int main(int argc, char* argv[]) {
    const auto vers = app::Version();

    auto config = app::config::parse_cli(argc, argv);
    spdlog::info("DataLogger Version: {}", vers.to_string());

    // now start the web/http listener
    auto ok = app::run_service(config);
    spdlog::info("Server shutdown, code: {}.", ok);

    return 0;
}
