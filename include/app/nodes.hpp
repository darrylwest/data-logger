// nodes.hpp
#pragma once

#include <app/client.hpp>
#include <app/taskrunner.hpp>
#include <app/types.hpp>

namespace app {
    namespace nodes {
        using namespace app::taskrunner;
        using namespace app::client;

        // read the json config file to determine if the node is active
        bool is_node_active(const Str& location, const auto& jcfg);

        // create the task with a default of 5 minute period
        Task create_temps_task(ClientNode& node, int period = 300);
        void append_temps_tasks(Vec<ClientNode>& nodes, Vec<Task>& tasks);

        // create status with a 10 minute period
        Task create_status_task(ClientNode& node, int period = 600);
        void append_status_tasks(Vec<ClientNode>& nodes, Vec<Task>& tasks);

        // create backup with a 60 minute period
        // Task create_backup_task(app::database::Database db, int period = 3600)

    }  // namespace nodes
}  // namespace app
