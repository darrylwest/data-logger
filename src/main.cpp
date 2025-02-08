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

    // define and start the non-data ticker + jobs
    // start_tasks(tasks);

    // used for graceful shutdown
    // halt_threads.clear();

    // now start the web/http listener
    auto ok = app::run_service(config);
    spdlog::info("Server shutdown, code: {}.", ok);

    // shut down the ticker tasks
    // halt_threads.test_and_set();

    return 0;
}
