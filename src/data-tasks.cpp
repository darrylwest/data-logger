//
// 2025-02-07 15:47:16 dpw
//

#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/version.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/types.hpp>

// special
#include <app/client.hpp>
#include <app/nodes.hpp>
#include <thread>

#include <iostream>
#include <csignal>

using namespace app::taskrunner;

void signal_handler(int signal) {
    if (signal == SIGSEGV) {
        std::cerr << "Segmentation fault!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    std::signal(SIGSEGV, signal_handler);
    const auto vers = app::Version();

    auto config = app::parse_cli(argc, argv);
    spdlog::info("DataTasks Version: {}", vers.to_string());

    // define and start the ticker + jobs
    // auto tasks = getTasks();
    // start_tasks(tasks);
    // list: fetch_temp_readings, fetch_other_readings, fetch_client_status
    auto nodes = app::client::create_nodes();
    Vec<Task> tasks;

    // add all tasks to nodes
    app::nodes::append_temps_tasks(nodes, tasks);
    app::nodes::append_status_tasks(nodes, tasks);

    // used for graceful shutdown
    halt_threads.clear();

    auto tlist = start_tasks(tasks);

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
