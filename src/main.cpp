
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/version.hpp>
#include <vector>

// special
#include <app/client.hpp>
#include <app/nodes.hpp>
#include <thread>
#include <app/datetime.hpp>

using namespace app::taskrunner;

int main(int argc, char* argv[]) {
    const auto vers = app::Version();
    const auto local = datetime::get_local_datetime();

    spdlog::info("local datetime: {}", local);

    auto config = app::parse_cli(argc, argv);
    spdlog::info("DataLogger Version: {}", vers.to_string());

    // define and start the ticker + jobs
    // auto tasks = getTasks();
    // start_tasks(tasks);
    // list: fetch_temp_readings, fetch_other_readings, fetch_client_status
    auto nodes = app::client::create_nodes();
    std::vector<Task> tasks;

    // add all tasks to nodes
    app::nodes::append_temps_tasks(nodes, tasks);
    app::nodes::append_status_tasks(nodes, tasks);

    // used for graceful shutdown
    halt_threads.clear();

    auto tlist = start_tasks(tasks);

    // now start the web/http listener
    auto ok = app::run_service(config);
    spdlog::info("Server shutdown, code: {}.", ok);

    // shut down the ticker tasks
    halt_threads.test_and_set();
    for (auto& t : tlist) {
        t.join();
    }

    return 0;
}
