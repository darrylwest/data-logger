// nodes.cpp
#include <spdlog/spdlog.h>

#include <app/nodes.hpp>
#include <app/taskrunner.hpp>

namespace app {
    namespace nodes {
        using namespace app::taskrunner;

        Task create_temps_task(app::client::ClientNode& node, int period) {
            auto worker = [&]() {
                auto data = app::client::fetch_temps(node);
                int ts = timestamp_seconds();
                node.last_access = ts;
                spdlog::info("ts: {}, temps: {}", ts, data.to_string());
                // send to db with timestamp
                // db.put(data);
            };

            auto task = createTask(node.location.c_str(), period, worker);
            return task;
        }

        std::vector<Task> create_temps_task_list(std::vector<app::client::ClientNode>& nodes) {
            std::vector<Task> tasks;
            for (auto& node : nodes) {
                tasks.push_back(create_temps_task(node));
            }
            return tasks;
        }

    }  // namespace nodes
}  // namespace app
