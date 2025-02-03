// nodes.hpp
#pragma once

#include <app/client.hpp>
#include <app/taskrunner.hpp>
#include <vector>

namespace app {
    namespace nodes {
        using namespace app::taskrunner;
        using namespace app::client;

        Task create_temps_task(app::client::ClientNode& node, int period = 10);
        std::vector<Task> create_temps_task_list(std::vector<ClientNode>& nodes);
    }  // namespace nodes
}  // namespace app
