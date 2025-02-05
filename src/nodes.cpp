// nodes.cpp
#include <spdlog/spdlog.h>
#include <vector>

#include <app/nodes.hpp>
#include <app/taskrunner.hpp>

namespace app {
    namespace nodes {
        using namespace app::taskrunner;

        Task create_temps_task(app::client::ClientNode& node, int period) {
            // create a db for this task based on temps and node.location
            // filename = data/temps/probe-location/day-date.detail

            auto worker = [&]() mutable {
                auto data = app::client::fetch_temps(node);
                int ts = timestamp_seconds();

                spdlog::info("ts {}, temps: {}", ts, data.to_string());

                for (const auto probe : data.probes) {
                    // db.(key, data);
                    spdlog::info("probe: {} = {}", probe.location, probe.tempC);
                }

                // append the database file

                node.last_access = ts;
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
