//
// g++ -std=c++20 -pthread -lpthread -Wall -o task-runner task-runner.cpp
//

#include <sstream>
#include <thread>

#include <spdlog/spdlog.h>

unsigned int timestamp_seconds() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    return timestamp;
}

void run(std::function<void()> func, int period) {
    bool keep_running = true;
    int run_count = 0;
    int last_run = timestamp_seconds();

    using namespace std::chrono;
    auto next = steady_clock::now();

    while (keep_running) {
        func();
        
        run_count++;
        int ts = timestamp_seconds();
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

}

std::thread start_task(std::function<void()> func, int period) {
    std::thread t(run, func, period);

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

    auto t1 = start_task(worker, 10);
    
    // wait for the thread to complete
    t1.join();


    return 0;
}
