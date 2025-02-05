// nodes.cpp
#include <spdlog/spdlog.h>

#include <app/nodes.hpp>
#include <app/taskrunner.hpp>
#include <vector>

namespace app {
    namespace nodes {
        using namespace app::taskrunner;

        Task create_temps_task(app::client::ClientNode& node, int period) {
            std::map<std::string, float> cache;

            auto worker = [cache = std::move(cache), &node = node]() mutable {
                auto data = app::client::fetch_temps(node);
                int ts = timestamp_seconds();

                // save data from each probe
                for (const auto probe : data.probes) {
                    std::string key = probe.location + std::to_string(ts);
                    cache[key] = probe.tempC;
                }

                int minute = ts / 60;
                if (minute != (node.last_access / 60)) {
                    spdlog::info("save to database at minute = {}", minute);
                    for (const auto& [key, value] : cache) {
                        spdlog::info("{}={}", key, value);
                    }
                    cache.clear();
                }
                node.last_access = ts;
                spdlog::info("temps: {}", ts, data.to_string());
                // db.(key, data);
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
