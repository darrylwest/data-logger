//
// 2025-02-07 15:47:16 dpw
//

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <unistd.h>

#include <app/cli.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/types.hpp>
#include <app/version.hpp>

// special
#include <app/client.hpp>
#include <app/nodes.hpp>
#include <iostream>
#include <thread>
#include <iostream>

using namespace app::taskrunner;
constexpr int MAX_LOG_SIZE = 5000000;
constexpr int MAX_LOG_SAVE = 5; // days

void configure_logging(const Str& logfile) {
    auto logger = spdlog::rotating_logger_mt("rotating_logger", logfile, MAX_LOG_SIZE, MAX_LOG_SAVE);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::info);
}

int main(int argc, char* argv[]) {
    const auto vers = app::Version();
    Str logfile = "logs/data-tasks.log";
    fmt::println("Starting data-tasks, Version: {}, logging at {}", vers.to_string(), logfile);

    configure_logging(logfile);

    auto config = app::config::parse_cli(argc, argv);
    spdlog::info("DataTasks Version: {}", vers.to_string());

    // define and start the ticker + jobs
    // auto tasks = getTasks();
    // start_tasks(tasks);
    // list: fetch_temp_readings, fetch_other_readings, fetch_client_status
    auto nodes = app::client::create_nodes();
    spdlog::info("configured nodes, count: {}", nodes.size());
    Vec<Task> tasks;

    // add all tasks to nodes
    app::nodes::append_temps_tasks(nodes, tasks);
    app::nodes::append_status_tasks(nodes, tasks);
    // TODO add a post to server task to post readings directly to data-logger

    // used for graceful shutdown
    halt_threads.clear();

    auto tlist = start_tasks(tasks);
    spdlog::info("configured tasks, count: {}", nodes.size());

    // read a shutdown hook from unix socket or signal
    //
    // shut down the ticker tasks
    //
    // halt_threads.test_and_set();

    for (auto& t : tlist) {
        t.join();
    }

    return 0;
}
