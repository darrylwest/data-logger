
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/version.hpp>
#include <vector>

// special
#include <app/client.hpp>
#include <vendor/taskrunner.hpp>
#include <thread>

// TODO move this once it's semi-stable
namespace app {
    namespace nodes {
        using namespace app::client;

        // TODO read from config
        std::vector<ClientNode> createNodes() {
            std::vector<ClientNode> nodes;
            auto node = ClientNode{
                .location = "cottage",
                .ip = "10.0.1.197",
                .port = 2030,
                .active = true,
                .last_access = 0,
            };

            nodes.push_back(node);

            return nodes;
        }

        taskrunner::Task createNodeTask(ClientNode& node) {
            auto worker = [&node]() {
                auto data = app::client::fetch_temps(node);
                spdlog::info("temps: {}", data.to_string());
            };
            
            auto task = taskrunner::createTask(node.location.c_str(), 10, worker);

            return task;
        }
    }
}

int main(int argc, char *argv[]) {
    const auto vers = app::Version();

    auto config = app::parse_cli(argc, argv);
    spdlog::info("DataLogger Version: {}", vers.to_string());

    // define and start the ticker + jobs
    // auto tasks = getTasks();
    // taskrunner::start_tasks(tasks);
    // list: fetch_readings, save_database, fetch_client_status, backup_database, init_database
    auto nodes = app::nodes::createNodes();
    auto task = app::nodes::createNodeTask(nodes.at(0));
    spdlog::info("task: {}", task.to_string());
    // taskrunner::start(task);
    std::thread t1(taskrunner::run, task.runner, task.name.c_str(), task.period);

    // now start the listener
    auto ok = app::run_service(config);
    spdlog::info("Server shutdown, code: {}.", ok);

    taskrunner::halt_threads.test_and_set();
    t1.join();

    return 0;
}
