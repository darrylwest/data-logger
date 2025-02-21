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
#include <fstream>
#include <nlohmann/json.hpp>

namespace app {
    namespace cfgsvc {
        using json = nlohmann::json;

        struct ServiceContext {
            std::string cfg_filename = "../config/config.json";
            std::chrono::seconds sleep_duration{10}; // Sleep time between work
        };

        class ConfigService {
        public:
            static ConfigService& instance() {
                static ConfigService service;
                return service;
            }

            json web_config() {
                spdlog::info("return the web config");
                std::lock_guard<std::mutex> lock(mtx);
                json j;

                j["webserver"]["host"] = "http://example.com";

                return j;
            }

            json client_config(const std::string& client_name) {
                spdlog::info("return the client config");
                std::lock_guard<std::mutex> lock(mtx);
                json j;

                j[client_name]["host"] = "http://" + client_name;
                j[client_name]["port"] = 9090;

                return j;
            }

            static void configure(const ServiceContext& ctx) {
                spdlog::info("reconfigure the instance");
                auto& service = instance();
                std::lock_guard<std::mutex> lock(service.mtx);
                service.ctx = ctx;
                service.start_worker();
            }

            // Destructor ensures clean shutdown
            ~ConfigService() {
                spdlog::info("deconstruct: stop the worker");
                stop_worker();
            }

        private:
            ConfigService() = default; // this ensures it's a singleton
            ConfigService(const ConfigService&) = delete; // prevents copying
            ConfigService& operator=(const ConfigService&) = delete; // prevents re-assignment

            void start_worker() {
                spdlog::info("start the worker");
                running = true;
                worker = std::thread([this]() { worker_loop(); });
            }

            void stop_worker() {
                spdlog::info("stop the worker");
                running = false;
                if (worker.joinable()) {
                    worker.join();
                }
            }

            void worker_loop() {
                int loops = 0;
                while (running) {
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        loops++;
                        spdlog::info("read the config file: {}", loops);

                        // readd
                        std::ifstream fin(ctx.cfg_filename);
                        json data = json::parse(fin);
                        spdlog::info("{}", data.dump());
                    }
                    
                    std::this_thread::sleep_for(ctx.sleep_duration);
                }
            }

            ServiceContext ctx;
            std::mutex mtx;
            std::thread worker;
            std::atomic<bool> running{false};
        };

        // public interface
        json web_config() {
            return ConfigService::instance().web_config();
        }

        json client_config(const std::string& client_name) {
            return ConfigService::instance().client_config(client_name);
        }

        inline void configure(const ServiceContext& config) {
            ConfigService::configure(config);
        }
    }
}

int main() {
    using namespace app;
    using json = nlohmann::json;

    // Configure service with custom sleep duration
    cfgsvc::ServiceContext ctx;
    ctx.sleep_duration = std::chrono::seconds(3);
    cfgsvc::configure(ctx);

    json j = cfgsvc::web_config();
    spdlog::info("result: {}", j.dump());


    j = cfgsvc::client_config("deck.com");
    spdlog::info("result: {}", j.dump());

    std::this_thread::sleep_for(std::chrono::seconds(4));
    j = cfgsvc::web_config();
    spdlog::info("result: {}", j.dump());


    j = cfgsvc::client_config("cottage.com");
    spdlog::info("result: {}", j.dump());


    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}
