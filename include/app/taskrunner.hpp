//
//
// 2024-12-31 10:28:16 dpw
// run tasks as controlled by a real-time ticker
// intended for tasks that run each few seconds or minutes/hours, etc.
//
//

#pragma once

#include <app/types.hpp>
#include <atomic>
#include <iostream>
#include <sstream>
#include <thread>

namespace app {
    namespace taskrunner {
        extern std::atomic_flag halt_threads;

        struct Task {
            const char* name;
            const std::time_t started_at;  // unix timestamp in seconds
            int last_run;                  // unix timestamp in seconds
            int period;                    // in seconds
            unsigned long run_count;
            Func<void()> runner;

            friend std::ostream& operator<<(std::ostream& os, const Task v) {
                // better to use <format> but it breaks on linux and fmt broken on darwin
                os << v.name << ", started_at: " << v.started_at << ", last_run: " << v.last_run
                   << ", period: " << v.period << ", run_count: " << v.run_count;

                return os;
            }

            Str to_string() const {
                std::ostringstream oss;
                oss << *this;

                return oss.str();
            }
        };

        // create the task
        Task createTask(const char* task_name, int period, Func<void()> task_runner);

        // run the func; if period == 0 then this is a one-shot-and-out task
        void run(const Func<void()> func, const char* name, const int period);

        // start a thread using the task values
        std::thread start(const Task& task);

        // start a list of threads; clears the halt_threads flag
        Vec<std::thread> start_tasks(const Vec<Task> list);
    }  // namespace taskrunner
}  // namespace app
