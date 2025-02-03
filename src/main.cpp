
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/version.hpp>
#include <vector>

int main(int argc, char *argv[]) {
    const auto vers = app::Version();

    auto config = app::parse_cli(argc, argv);
    spdlog::info("DataLogger Version: {}", vers.to_string());

    // define and start the ticker + jobs
    // auto tasks = getTasks();
    // taskrunner::start_tasks(tasks);
    // list: fetch_readings, save_database, fetch_client_status, backup_database, init_database

    // now start the listener
    auto ok = app::run_service(config);
    spdlog::info("Server shutdown, code: {}.", ok);

    return 0;
}
