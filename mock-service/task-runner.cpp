//
// g++ -std=c++20 -pthread -lpthread -Wall -o task-runner task-runner.cpp
//

#include <sstream>
#include <thread>
#include <atomic>

#include <spdlog/spdlog.h>
#include "../include/vendor/taskrunner.hpp"

static std::atomic_flag halt_threads;

void run(const std::function<void()> func, const char* name, const int period) {

    int error_count = 0;
    int last_run = taskrunner::timestamp_seconds();
    int run_count = 0;

    using namespace std::chrono;
    auto next = steady_clock::now();

    try {
        while (!halt_threads.test() && error_count < 5) {
            func();
        
            run_count++;
            int ts = taskrunner::timestamp_seconds();
            int delta = ts - last_run;

            if (run_count > 1 && delta != period) {
                error_count++;
                spdlog::error("DELTA ERROR! count: {}, last: {}, delta: {}", run_count, last_run, delta);
            } else {
                spdlog::info("count: {}, last: {}, delta: {}", run_count, last_run, delta);
            }

            last_run = ts;

            next += seconds(period);

            std::this_thread::sleep_until(next);
        }
    } catch (std::exception& e) {
        spdlog::error("Fatal error running task: {}, {}", name, e.what());
        throw e;
    }

}

std::thread start_task(taskrunner::Task& task) {
    std::thread t(run, task.runner, task.name.c_str(), task.period);
    // spdlog::info("started task thread: {} for {}", t.get_id(), task.name);

    return t;
}

int main() {
    halt_threads.clear();

    int counter = 0;
    auto worker = [&counter]() {
        counter++;
        spdlog::info("counter: {}", counter);
        
        // simulate a long task
        std::this_thread::sleep_for(std::chrono::milliseconds(850));
    };

    auto task = taskrunner::createTask("test", 10, worker);
    auto t1 = start_task(task);

    spdlog::info("Thread started for task: {}", task.name);

    if (false) {
        std::this_thread::sleep_for(std::chrono::seconds(62));
        spdlog::warn("shutdown called, shutting down threads...");
        halt_threads.test_and_set();
    }
    
    // wait for the thread to complete
    t1.join();

    spdlog::info("thread joined.");


    return 0;
}
