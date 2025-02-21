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
    namespace cfgsvc {

        struct ServiceContext {
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
                std::lock_guard<std::mutex> lock(mtx);

                return data + "-> " + std::to_string(ctx.param2);
            }

            static void configure(const ServiceContext& ctx) {
                spdlog::info("reconfigure the instance");
                auto& service = instance();
                std::lock_guard<std::mutex> lock(service.mtx);
                service.ctx = ctx;
                service.start_worker();
            }

            // Destructor ensures clean shutdown
            ~MyService() {
                spdlog::info("deconstruct: stop the worker");
                stop_worker();
            }

        private:
            MyService() = default; // this ensures it's a singleton
            MyService(const MyService&) = delete; // prevents copying
            MyService& operator=(const MyService&) = delete; // prevents re-assignment

            void start_worker() {
                spdlog::info("start the worker");
                running = true;
                worker = std::thread([this]() { workerLoop(); });
            }

            void stop_worker() {
                spdlog::info("stop the worker");
                running = false;
                if (worker.joinable()) {
                    worker.join();
                }
            }

            void workerLoop() {
                int loops = 0;
                while (running) {
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        loops++;
                        spdlog::info("do the work, loop: {}", loops);

                        // change something here...
                        ctx.param2++;
                    }
                    
                    std::this_thread::sleep_for(ctx.sleep_duration);
                }
            }

            ServiceContext ctx;
            std::mutex mtx;
            std::thread worker;
            std::atomic<bool> running{false};
        };

        template<typename... Args>
        std::string do_something(Args&&... args) {
            return MyService::instance().doSomething(std::forward<Args>(args)...);
        }

        inline void configure(const ServiceContext& config) {
            MyService::configure(config);
        }
    }
}

int main() {
    using namespace app;

    // Configure service with custom sleep duration
    cfgsvc::ServiceContext config;
    config.sleep_duration = std::chrono::milliseconds(1500);
    cfgsvc::configure(config);

    // Service will run in background
    std::string r = cfgsvc::do_something("hello");
    spdlog::info("result: {}", r);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    r = cfgsvc::do_something("hello again");
    spdlog::info("result: {}", r);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    r = cfgsvc::do_something("hello once more");
    spdlog::info("result: {}", r);

    // Your main program continues...
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::this_thread::sleep_for(std::chrono::seconds(3));
    r = cfgsvc::do_something("over and out");
    spdlog::info("result: {}", r);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
