//
//
// 2024-12-31 10:28:16 dpw
//
//

#include <spdlog/spdlog.h>

#include <app/taskrunner.hpp>
#include <atomic>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

namespace app {
    namespace taskrunner {
        std::atomic_flag halt_threads = ATOMIC_FLAG_INIT;

        // get the timestamp in millis
        unsigned long timestamp_millis() {
            auto now = std::chrono::system_clock::now();
            auto timestamp
                = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch())
                      .count();

            return timestamp;
        }

        // get the timestamp in seconds
        unsigned int timestamp_seconds() {
            auto now = std::chrono::system_clock::now();
            auto timestamp
                = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

            return timestamp;
        }

        // create the task
        Task createTask(const char* task_name, int period, std::function<void()> task_runner) {
            auto ts = timestamp_seconds();
            auto task = Task{
                .name = task_name,
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
            // re-create the task for easy reporting
            auto task = createTask(name, period, func);

            task.run_count = 0;

            using namespace std::chrono;
            auto next = steady_clock::now();

            task.last_run = taskrunner::timestamp_seconds();
            int tick_count = period;

            try {
                while (!halt_threads.test()) {
                    if (tick_count >= period) {
                        func();
                        int ts = taskrunner::timestamp_seconds();

                        if (period == 0) {
                            spdlog::info("{} is a one-shot task, complete.", name);
                            return;
                        }

                        int delta = ts - task.last_run;

                        task.run_count++;

                        if (task.run_count > 1 && std::abs(delta - period) > 1) {
                            spdlog::warn("DELTA ERROR! task: {}, delta: {}", task.to_string(),
                                         delta);
                        } else {
                            spdlog::info("task: {}, delta: {}", task.to_string(), delta);
                        }

                        task.last_run = ts;
                        tick_count = 0;
                    }

                    // always increment
                    tick_count++;

                    next += seconds(1);

                    std::this_thread::sleep_until(next);
                }
            } catch (std::exception& e) {
                spdlog::error("Fatal error running task: {}, {}", name, e.what());
                throw e;
            }
        }

        // start a thread using the task values
        std::thread start(const Task& task) {
            std::thread t(run, task.runner, task.name, task.period);

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
    }  // namespace taskrunner
}  // namespace app
