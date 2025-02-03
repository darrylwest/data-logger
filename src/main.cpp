
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/version.hpp>
#include <vector>

// special
#include <app/client.hpp>
#include <thread>
#include <vendor/taskrunner.hpp>

// TODO move this once it's semi-stable
namespace app {
    namespace nodes {
        using namespace app::client;
        using namespace taskrunner;

        taskrunner::Task create_temps_task(ClientNode& node, const int period = 10) {
            // store in local db to get averages; store average to database
            auto worker = [&node]() {
                auto data = app::client::fetch_temps(node);
                node.last_access = taskrunner::timestamp_seconds();
                // TODO store to database
                spdlog::info("temps: {}", data.to_string());
            };

            auto task = taskrunner::createTask(node.location.c_str(), period, worker);

            return task;
        }

        std::vector<Task> create_temps_task_list(std::vector<ClientNode>& nodes) {
            std::vector<taskrunner::Task> tasks;

            for (auto& node : nodes) {
                tasks.push_back(create_temps_task(node));
            }

            return tasks;
        }

    }  // namespace nodes
}  // namespace app

int main(int argc, char* argv[]) {
    const auto vers = app::Version();

    auto config = app::parse_cli(argc, argv);
    spdlog::info("DataLogger Version: {}", vers.to_string());

    // define and start the ticker + jobs
    // auto tasks = getTasks();
    // taskrunner::start_tasks(tasks);
    // list: fetch_readings, save_database, fetch_client_status, backup_database, init_database
    auto nodes = app::client::create_nodes();
    // auto task = app::nodes::create_temps_task(nodes.at(0));

    // std::vector<taskrunner::Task> tasks;
    auto tasks = app::nodes::create_temps_task_list(nodes);

    taskrunner::halt_threads.clear();

    auto tlist = taskrunner::start_tasks(tasks);

    // now start the web/http listener
    auto ok = app::run_service(config);
    spdlog::info("Server shutdown, code: {}.", ok);

    // shut it down
    taskrunner::halt_threads.test_and_set();
    for (auto& t : tlist) {
        t.join();
    }

    return 0;
}
