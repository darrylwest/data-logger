// nodes.cpp
#include <app/nodes.hpp>
#include <spdlog/spdlog.h>
#include <vendor/taskrunner.hpp>

namespace app {
    namespace nodes {

        taskrunner::Task create_temps_task(app::client::ClientNode& node, int period) {
            auto worker = [&node]() {
                auto data = app::client::fetch_temps(node);
                node.last_access = taskrunner::timestamp_seconds();
                spdlog::info("temps: {}", data.to_string());
            };

            auto task = taskrunner::createTask(node.location.c_str(), period, worker);
            return task;
        }

        std::vector<taskrunner::Task> create_temps_task_list(std::vector<app::client::ClientNode>& nodes) {
            std::vector<taskrunner::Task> tasks;
            for (auto& node : nodes) {
                tasks.push_back(create_temps_task(node));
            }
            return tasks;
        }

    }  // namespace nodes
}  // namespace app

