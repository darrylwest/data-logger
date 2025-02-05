// nodes.hpp
#pragma once

#include <app/client.hpp>
#include <app/taskrunner.hpp>
#include <vector>

namespace app {
    namespace nodes {
        using namespace app::taskrunner;
        using namespace app::client;

        // create the task with a default of 15 second period
        Task create_temps_task(app::client::ClientNode& node, int period = 15);
        std::vector<Task> create_temps_task_list(std::vector<ClientNode>& nodes);

        // Task create_status_task(app::client::ClientNode& node, int period = 0)
        // Task create_backup_task(app::database::Database db, int period = 0)

    }  // namespace nodes
}  // namespace app
