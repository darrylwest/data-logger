//
// test the task runner
// g++ -std=c++20 -o task-runner task-runner.cpp
//

#include <sstream>
#include <thread>

#include <spdlog/spdlog.h>
#include "../include/vendor/taskrunner.hpp"


int main() {
    // create the task
    //
    int counter = 0;
    auto worker = [&counter]() {
        counter++;
        spdlog::info("counter: {}", counter);
    };

    auto task = taskrunner::createTask("test-task", 1, worker);

    taskrunner::run(task);

    return 0;
}
