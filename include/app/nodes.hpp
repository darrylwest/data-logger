// nodes.hpp
#pragma once

#include <vector>
#include <app/client.hpp>
#include <vendor/taskrunner.hpp>

namespace app {
    namespace nodes {
        taskrunner::Task create_temps_task(app::client::ClientNode& node, int period = 10);
        std::vector<taskrunner::Task> create_temps_task_list(std::vector<app::client::ClientNode>& nodes);
    }
}

