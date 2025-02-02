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

#include <spdlog/spdlog.h>

namespace taskrunner {
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

    void run(Task& task) {
        spdlog::info("starting task: {}, period: {} seconds.", task.name, task.period);

        try {
            auto now = std::chrono::steady_clock::now();
            task.runner();
            task.run_count++;

            using std::chrono::operator""ms;
            auto next = now + (task.period * 1000ms);

            std::this_thread::sleep_until(next);
        } catch (std::exception& e) {
            spdlog::error("Fatal error running task: {}, {}", task.name, e.what());
            throw e;
        }
    }
}  // namespace app
