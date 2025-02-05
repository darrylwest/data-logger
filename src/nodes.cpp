// nodes.cpp
#include <spdlog/spdlog.h>

#include <app/database.hpp>
#include <app/nodes.hpp>
#include <app/taskrunner.hpp>
#include <vector>

namespace app {
    namespace nodes {
        using namespace app::taskrunner;

        Task create_temps_task(app::client::ClientNode& node, int period) {
            // create a db for this task based on temps and node.location
            // filename = data/temperature/probe-location/day-date.detail

            auto worker = [&]() mutable {
                auto data = app::client::fetch_temps(node);
                int ts = timestamp_seconds();

                std::string at = app::database::truncate_to_minutes(data.reading_at);
                spdlog::info("ts {}, temps: {}, at: {}", ts, data.to_string(), at);

                for (const auto probe : data.probes) {
                    // db.(key, data);
                    auto key = app::database::create_key(at, probe.location);

                    // TODO create a method for this? pull data folder from config...
                    const auto filename = "data/temperature/current." + probe.location + ".db";
                    spdlog::info("file: {}, k/v: {}={}", filename, key.to_string(), probe.tempC);

                    app::database::append_key_value(filename, key, std::to_string(probe.tempC));
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
