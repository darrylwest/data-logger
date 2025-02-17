//
// 2025-02-07 15:47:16 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <app/cli.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/types.hpp>
#include <app/version.hpp>
#include <vendor/ansi_colors.hpp>

// special
#include <app/client.hpp>
#include <app/nodes.hpp>
#include <iostream>
#include <thread>

using namespace app::taskrunner;
constexpr int LOG_SIZE = 1'000'000;
constexpr int LOG_SAVE = 5;  // days

void configure_logging(const Str& logfile) {
    auto logger = spdlog::rotating_logger_mt("rotating_logger", logfile, LOG_SIZE, LOG_SAVE);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::info);
}

int main(int argc, char* argv[]) {
    // get the pid; write to data-tasks.pid
    int pid = getpid();

    const auto vers = app::Version();
    Str logfile = "logs/data-tasks.log";
    fmt::println("{}Starting data-tasks, Version: {}, logging at {}, PID: {}{}", colors::cyan,
                 vers.to_string(), logfile, pid, colors::reset);

    configure_logging(logfile);
    spdlog::info("Started DataTasks, PID: {}", pid);

    auto config = app::config::parse_cli(argc, argv);
    spdlog::info("DataTasks Version: {}", vers.to_string());

    // define and start the ticker + jobs
    // list: fetch_temp_readings, fetch_other_readings, fetch_client_status, etc
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
