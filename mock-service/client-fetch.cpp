//
// g++ -std=c++20 -pthread -lpthread -Wall -o client-fetch client-fetch.cpp
//

#include <httplib.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <thread>
#include <chrono>

static std::atomic_flag halt_threads;

void fetch_data() {
    while (!halt_threads.test()) {
        // Create a client pointing to the host and port
        std::string url = "http://10.0.1.199:2030";
        httplib::Client client(url);

        auto timeout = std::chrono::milliseconds{2000};
        client.set_connection_timeout(timeout);

        auto path = "/temps";
        spdlog::info("1: fetch to {}{}", url, path);

        // Send a GET request to the desired endpoint
        if (auto res = client.Get(path)) {  // Example API returning JSON
            if (res->status == 200) {
                std::cout << res->body << std::flush;
            } else {
                std::cerr << "Request failed with status: " << res->status << std::endl;
                spdlog::error("1: failed here");
            }
        } else {
            // Handle connection errors
            auto err = res.error();
            std::cerr << "Connection error: " << httplib::to_string(err) << std::endl;
            spdlog::error("2: failed here");
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main() {
    halt_threads.clear();

    std::thread t(fetch_data);

    t.join();

    return 0;
}

