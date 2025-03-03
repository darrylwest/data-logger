//
// nodes.cpp
//
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cfgsvc.hpp>
#include <app/cli.hpp>
#include <app/database.hpp>
#include <app/jsonkeys.hpp>
#include <app/nodes.hpp>
#include <datetimelib/datetimelib.hpp>

namespace app::nodes {
    using namespace app::taskrunner;
    using namespace app::client;

    // return true if this node is active based on current config
    bool is_node_active(const Str& location, const auto& jcfg) {
        using namespace app::jsonkeys;
        for (const auto& jclient : jcfg[CLIENTS]) {
            if (jclient[LOCATION] == location) {
                bool active = jclient[ACTIVE].template get<bool>();
                spdlog::info("client node: {} active: {}", location, active);
                return active;
            }
        }

        spdlog::error("could not find client node for location: {}", location);

        return false;
    }

    // create the temps worker task
    Task create_temps_task(ClientNode& node, int period) {
        auto worker = [&]() mutable {
            try {
                // read config for this node
                auto data = app::client::fetch_temps(node);

                spdlog::info("temps: {}, at: {}", data.to_string(), data.reading_at);

                // TODO get port from jcfg
                const auto wcfg = cfgsvc::webservice();
                const auto cfg = app::config::webconfig_from_json(wcfg);

                const Str host = (cfg.host == "0.0.0.0") ? "localhost" : cfg.host;
                const auto url = fmt::format("{}://{}:{}", cfg.scheme, host, cfg.port);

                // TODO get file path from config; use the client location, not the probe
                const FilePath path = "data/temperature/current." + node.location + ".db";

                for (const auto& probe : data.probes) {
                    // can be disabled by the remote client
                    if (!probe.enabled) {
                        spdlog::warn("probe disabled: {} {}", probe.sensor, probe.location);
                        continue;
                    }

                    // can be disabled by through configuration
                    // TODO - find the probe from jcfg to get enabled

                    auto key = app::database::create_key(data.reading_at,
                                                         fmt::format("{}.{}", node.location, probe.sensor));

                    spdlog::info("file: {}, {}={}", path.string(), key.to_string(), probe.tempC);

                    app::database::append_key_value(path, key, std::to_string(probe.tempC));

                    spdlog::info("put data to webserver: {}", url);
                    if (!app::client::put_temps(url, key, probe)) {
                        spdlog::warn("web service down at: {}", url);
                    }
                }

                node.last_access = datetimelib::timestamp_seconds();
            } catch (std::exception& e) {
                spdlog::error("worker: {} data access failed: {}", node.location, e.what());
            }
        };

        auto task = createTask(node.location.c_str(), period, worker);
        return task;
    }

    // create a list of temps tasks from the node list
    void append_temps_tasks(Vec<ClientNode>& nodes, Vec<Task>& tasks) {
        for (auto& node : nodes) {
            tasks.push_back(create_temps_task(node));
        }
    }

    // create a task to query the client status
    Task create_status_task(ClientNode& node, const int period) {
        auto worker = [&]() {
            try {
                // read config to see if the client is still active
                if (!node.active) {
                    return;
                }

                const auto status = app::client::fetch_status(node);

                const auto isodate = datetimelib::ts_to_local_isodate(status.timestamp);
                spdlog::info("ts: {}, uptime: {}, access: {}, errors: {}", status.timestamp, status.uptime,
                             status.access_count, status.errors);

                auto key = app::database::create_key(status.timestamp, node.location);
                const FilePath path = "data/status/current." + node.location + ".db";
                spdlog::info("file: {}, k/v: {}={}", path.string(), key.to_string(), status.to_string());

                app::database::append_key_value(path, key, status.to_string());
            } catch (const std::exception& e) {
                spdlog::error("worker: {} data access failed: {}", node.location, e.what());
            }
        };

        auto task = createTask(node.location.c_str(), period, worker);
        return task;
    }

    // create a list of status tasks from the node list
    void append_status_tasks(Vec<ClientNode>& nodes, Vec<Task>& tasks) {
        for (auto& node : nodes) {
            tasks.push_back(create_status_task(node));
        }
    }
}  // namespace app::nodes
