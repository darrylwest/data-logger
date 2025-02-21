//
// 2025-02-21 01:18:17 dpw
//
// this is a pattern for a generic service
//

#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <spdlog/spdlog.h>

namespace app {
    namespace config {

        struct ServiceConfig {
            std::string param1 = "default";
            int param2 = 0;
            std::chrono::milliseconds sleep_duration{1000}; // Sleep time between work
        };

        class MyService {
        public:
            static MyService& instance() {
                static MyService service;
                return service;
            }

            std::string doSomething(const std::string& data) {
                spdlog::info("do something with this data: {}", data);
                std::lock_guard<std::mutex> lock(mutex_);

                return data + "-> " + std::to_string(config_.param2);
            }

            static void configure(const ServiceConfig& config) {
                spdlog::info("reconfigure the instance");
                auto& service = instance();
                std::lock_guard<std::mutex> lock(service.mutex_);
                service.config_ = config;
                service.startWorker();
            }

            // Destructor ensures clean shutdown
            ~MyService() {
                spdlog::info("deconstruct: stop the worker");
                stopWorker();
            }

        private:
            MyService() = default; // this ensures it's a singleton
            MyService(const MyService&) = delete; // prevents copying
            MyService& operator=(const MyService&) = delete; // prevents re-assignment

            void startWorker() {
                spdlog::info("start the worker");
                running_ = true;
                worker_ = std::thread([this]() { workerLoop(); });
            }

            void stopWorker() {
                spdlog::info("stop the worker");
                running_ = false;
                if (worker_.joinable()) {
                    worker_.join();
                }
            }

            void workerLoop() {
                int loops = 0;
                while (running_) {
                    {
                        std::lock_guard<std::mutex> lock(mutex_);
                        loops++;
                        spdlog::info("do the work, loop: {}", loops);

                        // change something here...
                        config_.param2++;
                    }
                    
                    std::this_thread::sleep_for(config_.sleep_duration);
                }
            }

            ServiceConfig config_;
            std::mutex mutex_;
            std::thread worker_;
            std::atomic<bool> running_{false};
        };

        template<typename... Args>
        std::string do_something(Args&&... args) {
            return MyService::instance().doSomething(std::forward<Args>(args)...);
        }

        inline void configure(const ServiceConfig& config) {
            MyService::configure(config);
        }
    }
}

int main() {
    using namespace app;

    // Configure service with custom sleep duration
    config::ServiceConfig config;
    config.sleep_duration = std::chrono::milliseconds(1500);
    config::configure(config);

    // Service will run in background
    std::string r = config::do_something("hello");
    spdlog::info("result: {}", r);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    r = config::do_something("hello again");
    spdlog::info("result: {}", r);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    r = config::do_something("hello once more");
    spdlog::info("result: {}", r);

    // Your main program continues...
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::this_thread::sleep_for(std::chrono::seconds(3));
    r = config::do_something("over and out");
    spdlog::info("result: {}", r);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
