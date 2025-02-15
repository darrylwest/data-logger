
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <nlohmann/json.hpp>  // JSON library (https://github.com/nlohmann/json)
#include <httplib.h>          // Simple HTTP client (https://github.com/yhirose/cpp-httplib)

class DataBroadcaster {
public:
    DataBroadcaster() : stop_worker(false) {
        worker_thread = std::thread(&DataBroadcaster::process_data, this);
    }

    ~DataBroadcaster() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            stop_worker = true;
        }
        queue_cv.notify_one();
        worker_thread.join();
    }

    // Adds a new key/value pair to the queue
    void add_data(const std::string& key, const std::string& value) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            data_queue.emplace(key, value);
        }
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
                std::unique_lock<std::mutex> lock(queue_mutex);
                queue_cv.wait_for(lock, std::chrono::minutes(5), [this] { return !data_queue.empty() || stop_worker; });

                if (stop_worker && data_queue.empty()) break;

                while (!data_queue.empty()) {
                    batch.push_back(data_queue.front());
                    data_queue.pop();
                }
            }

            if (!batch.empty()) {
                // Convert batch data to JSON
                nlohmann::json json_payload;
                for (const auto& [key, value] : batch) {
                    json_payload[key] = value;
                }

                // Send batch request
                auto res = client.Post("/api/data", json_payload.dump(), "application/json");
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

