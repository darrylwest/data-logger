//
// NOTES:  don't like the batch thing; I need one at a time as a fire and forget...
//

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <nlohmann/json.hpp>
#include <httplib.h>
#include <spdlog/spdlog.h>

class DataBroadcaster {
public:
    DataBroadcaster() : stop_worker(false) {
        worker_thread = std::thread(&DataBroadcaster::process_data, this);
        spdlog::info("constructor: start the worker thread");
    }

    ~DataBroadcaster() {
        {
            spdlog::info("destructor: stop the worker thread");
            std::lock_guard<std::mutex> lock(queue_mutex);
            stop_worker = true;
        }
        queue_cv.notify_one();
        worker_thread.join();
    }

    // Adds a new key/value pair to the queue
    void add_data(const std::string& key, const std::string& value) {
        spdlog::info("add data: {} {}", key, value);
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            data_queue.emplace(key, value);
        }
        spdlog::info("queue size: {}", data_queue.size());
        queue_cv.notify_one();
    }

private:
    std::queue<std::pair<std::string, std::string>> data_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::thread worker_thread;
    bool stop_worker;

    void process_data() {
        httplib::Client client("http://example.com");

        while (true) {
            std::vector<std::pair<std::string, std::string>> batch;
            
            {
                spdlog::info("wait for lock");
                std::unique_lock<std::mutex> lock(queue_mutex);
                queue_cv.wait_for(lock, std::chrono::minutes(5), [this] { return !data_queue.empty() || stop_worker; });

                if (stop_worker && data_queue.empty()) break;

                while (!data_queue.empty()) {
                    spdlog::info("queue size: {}", data_queue.size());
                    batch.push_back(data_queue.front());
                    spdlog::info("batch size: {}", batch.size());
                    data_queue.pop();
                    spdlog::info("queue size: {}", data_queue.size());
                }
            }

            if (!batch.empty()) {
                // Convert batch data to JSON
                nlohmann::json json_payload;
                for (const auto& [key, value] : batch) {
                    json_payload[key] = value;
                }

                const auto body = json_payload.dump();
                spdlog::info("send data: {}", body);

                // Send batch request
                auto res = client.Post("/api/data", body, "application/json");
                if (res && res->status == 200) {
                    std::cout << "Batch successfully sent.\n";
                } else {
                    std::cerr << "Failed to send batch data.\n";
                }
            }
        }
    }
};

// Example usage:
int main() {
    DataBroadcaster broadcaster;

    // Simulating data collection
    for (int i = 0; i < 25; ++i) {
        broadcaster.add_data("temperature_" + std::to_string(i), std::to_string(20 + i) + "C");
    }

    std::this_thread::sleep_for(std::chrono::minutes(6));  // Simulate runtime
    return 0;
}

