//
// 2025-02-07 15:47:16 dpw
//
// Main data collection application
//   - mutli-threaded tasks
//   - logging includes thread id for filtering
//   - logs to rolling file
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <app/cli.hpp>
#include <app/types.hpp>
#include <app/version.hpp>
#include <cstdlib>
#include <datetimelib/datetimelib.hpp>
#include <vendor/ansi_colors.hpp>

#include "precompiled.hpp"

// special
#include <app/cfgsvc.hpp>
#include <app/client.hpp>
#include <app/nodes.hpp>
#include <iostream>
#include <thread>

using namespace app::taskrunner;
using namespace colors;
constexpr int LOG_SIZE = 1'000'000;
constexpr int LOG_SAVE = 5;  // days

void start_config_service() {
    using namespace app;

    cfgsvc::ServiceContext ctx;
    // add validators, funcs etc...

    // add the funcs and test before refactor

    cfgsvc::configure(ctx);
}

void configure_logging(const Str& logfile, const bool rolling) {
    if (rolling) {
        auto logger = spdlog::rotating_logger_mt("rotating_logger", logfile, LOG_SIZE, LOG_SAVE);
        spdlog::set_default_logger(logger);
    }
    spdlog::set_level(spdlog::level::info);
    spdlog::flush_on(spdlog::level::info);
}

int main(int argc, char* argv[]) {
    // set the log format first
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");

    // configuration service has to work or we are toast
    try {
        start_config_service();
    } catch (std::exception& e) {
        std::cerr << "Configuration died on startup, " << e.what() << ", aborting..." << std::endl;
        std::exit(1);
    }

    // add funcs

    // get the pid; write to data-tasks.pid
    int pid = getpid();

    const char* env_value = std::getenv("TESTING");
    bool testing = (env_value) ? true : false;

    const auto vers = app::Version();
    Str logfile = "logs/data-tasks.log";
    fmt::print("{}Starting data-tasks, Version: {}, logging at {}, PID: {}{}\n", cyan,
               vers.to_string(), logfile, pid, reset);

    const auto shutdown = [](int code) { exit(code); };
    configure_logging(logfile, !testing);
    spdlog::info("Started DataCollectionTasks, PID: {}", pid);

    const auto jconf = app::cfgsvc::webservice();
    const auto webconfig = app::config::webconfig_from_json(jconf);

    const auto params = app::config::CliParams{
        .argc = argc, .argv = argv, .config = webconfig, .shutdown = shutdown};

    auto config = app::config::parse_cli(params);
    spdlog::info("DataTasks Version: {}", vers.to_string());

    // define and start the ticker + jobs
    // list: fetch_temp_readings, fetch_other_readings, fetch_client_status, etc
    auto nodes = app::client::create_nodes();
    spdlog::info("configured nodes, count: {}", nodes.size());
    Vec<Task> tasks;

    // add all tasks to nodes
    app::nodes::append_temps_tasks(nodes, tasks);
    app::nodes::append_status_tasks(nodes, tasks);

    // // wait for the mark
    fmt::print("{}datetimelib version: {}{}\n", yellow, datetimelib::VERSION, reset);
    auto mp = datetimelib::MarkProvider();
    if (!testing) {
        datetimelib::wait_for_next_mark(mp, true);
    }

    fmt::print("{}ok, ready to start now...{}\n", green, reset);

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
