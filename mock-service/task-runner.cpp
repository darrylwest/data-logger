//
// g++ -std=c++20 -pthread -lpthread -Wall -o task-runner task-runner.cpp
//

#include <sstream>
#include <thread>

#include <spdlog/spdlog.h>
#include "../include/vendor/taskrunner.hpp"

void run(taskrunner::Task& task) {
    bool keep_running = true;

    while (keep_running) {
        auto now = std::chrono::steady_clock::now();
        task.last_run = taskrunner::timestamp_seconds();

        task.runner();
        task.run_count++;

        using std::chrono::operator""ms;
        auto next = now + (task.period * 1000ms);

        std::this_thread::sleep_until(next);
    }

}

std::thread start_task(taskrunner::Task& task) {
    std::thread t(run, task);

    return t;
}

int main() {
    
    int counter = 0;
    auto worker = [&counter]() {
        counter++;
        spdlog::info("counter: {}", counter);
        
        // simulate a long task
        std::this_thread::sleep_for(std::chrono::milliseconds(850));
    };

    int period = 10;
    auto task = taskrunner::createTask("test-task", period, worker);

    auto t1 = start_task(task);
    
    // wait for the thread to complete
    t1.join();


    return 0;
}
