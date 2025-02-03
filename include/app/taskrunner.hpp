//
//
// 2024-12-31 10:28:16 dpw
// run tasks as controlled by a real-time ticker
// intended for tasks that run each few seconds or minutes/hours, etc.
//
//

#pragma once

#include <atomic>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

namespace app {
    namespace taskrunner {
        extern std::atomic_flag halt_threads;

        // get the timestamp in millis
        unsigned long timestamp_millis();

        // get the timestamp in seconds
        unsigned int timestamp_seconds();

        struct Task {
            const char* name;
            const unsigned int started_at;  // unix timestamp in seconds
            int last_run;                   // unix timestamp in seconds
            int period;                     // in seconds
            unsigned long run_count;
            std::function<void()> runner;

            friend std::ostream& operator<<(std::ostream& os, const Task v) {
                // better to use <format> but it breaks on linux and fmt broken on darwin
                os << v.name << ", started_at: " << v.started_at << ", last_run: " << v.last_run
                   << ", period: " << v.period << ", run_count: " << v.run_count;

                return os;
            }

            std::string to_string() const {
                std::ostringstream oss;
                oss << *this;

                return oss.str();
            }
        };

        // create the task
        Task createTask(const char* task_name, int period, std::function<void()> task_runner);

        // run the func; if period == 0 then this is a one-shot-and-out task
        void run(const std::function<void()> func, const char* name, const int period);

        // start a thread using the task values
        std::thread start(const Task& task);

        // start a list of threads; clears the halt_threads flag
        std::vector<std::thread> start_tasks(const std::vector<Task> list);
    }  // namespace taskrunner
}  // namespace app
