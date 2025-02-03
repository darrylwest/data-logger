//
//
// 2024-12-31 10:28:16 dpw
// run tasks as controlled by a real-time ticker
// intended for tasks that run each few seconds or minutes/hours, etc.
//
//

#pragma once

#include <iostream>
#include <sstream>
#include <thread>
#include <atomic>
#include <vector>

#include <spdlog/spdlog.h>

namespace taskrunner {
    static std::atomic_flag halt_threads;

    // get the timestamp in millis
    unsigned long timestamp_millis() {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        return timestamp;
    }

    // get the timestamp in seconds
    unsigned int timestamp_seconds() {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

        return timestamp;
    }

    struct Task {
        const std::string name;
        const unsigned int started_at; // unix timestamp in seconds
        int last_run; // unix timestamp in seconds
        int period; // in seconds
        unsigned long run_count;
        std::function<void()> runner;

        friend std::ostream& operator<<(std::ostream& os, const Task v) {
            // better to use <format> but it breaks on linux and fmt broken on darwin
            os << v.name 
                << ", started_at: " << v.started_at 
                << ", last_run: " << v.last_run 
                << ", period: " << v.period 
                << ", run_count: " << v.run_count;

            return os;
        }

        std::string to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };

    // create the task
    Task createTask(const char* task_name, int period, std::function<void()> task_runner) {
        auto ts = timestamp_seconds();
        auto task = Task { 
            .name = std::string(task_name),
            .started_at = ts,
            .last_run = 0,
            .period = period,
            .run_count = 0,
            .runner = task_runner,
        };

        return task;
    }

    // run the func; if period == 0 then this is a one-shot-and-out task
    void run(const std::function<void()> func, const char* name, const int period) {

        int last_run = taskrunner::timestamp_seconds();
        int run_count = 0;

        using namespace std::chrono;
        auto next = steady_clock::now();

        try {
            while (!halt_threads.test()) {
                func();
                if (period == 0) {
                    spdlog::info("{} is a one-shot task, complete.", name);
                    return;
                }

                run_count++;
                int ts = taskrunner::timestamp_seconds();
                int delta = ts - last_run;

                if (run_count > 1 && delta != period) {
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

    std::thread start(const Task& task) {
        std::thread t(run, task.runner, task.name.c_str(), task.period);

        return t;
    }

    // start a list of threads; clears the halt_threads flag
    std::vector<std::thread> start_tasks(const std::vector<Task> list) {
        halt_threads.clear();
        std::vector<std::thread> tlist;
        for (auto task : list) {
            // move the thread into the list
            tlist.emplace_back(start(task));
        }

        return tlist;
    }
}  // namespace app
