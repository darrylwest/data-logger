// nodes.hpp
#pragma once

#include <app/client.hpp>
#include <app/taskrunner.hpp>
#include <vector>

namespace app {
    namespace nodes {
        using namespace app::taskrunner;
        using namespace app::client;

        // create the task with a default of 5 minute period
        Task create_temps_task(app::client::ClientNode& node, int period = 300);
        std::vector<Task> create_temps_task_list(std::vector<ClientNode>& nodes);

        // create status with a 10 minute period
        // Task create_status_task(app::client::ClientNode& node, int period = 600)

        // create backup with a 60 minute period
        // Task create_backup_task(app::database::Database db, int period = 3600)

    }  // namespace nodes
}  // namespace app
