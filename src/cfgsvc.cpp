//
// 2025-02-21 02:51:00 dpw
//
//

#include <app/cfgsvc.hpp>
#include <fstream>
#include <spdlog/spdlog.h>

namespace app {
    namespace cfgsvc {

        ConfigService& ConfigService::instance() {
            static ConfigService service;
            return service;
        }

        json ConfigService::web_config() {
            spdlog::info("return the web config");
            std::lock_guard<std::mutex> lock(mtx);
            json j;
            j["webserver"]["host"] = "http://example.com";
            return j;
        }

        json ConfigService::client_config(const std::string& client_name) {
            spdlog::info("return the client config");
            std::lock_guard<std::mutex> lock(mtx);
            json j;
            j[client_name]["host"] = "http://" + client_name;
            j[client_name]["port"] = 9090;
            return j;
        }

        void ConfigService::configure(const ServiceContext& ctx) {
            spdlog::info("reconfigure the instance");
            auto& service = instance();
            std::lock_guard<std::mutex> lock(service.mtx);
            service.ctx = ctx;
            service.start_worker();
        }

        ConfigService::ConfigService() = default;

        ConfigService::~ConfigService() {
            spdlog::info("deconstruct: stop the worker");
            stop_worker();
        }

        void ConfigService::start_worker() {
            spdlog::info("start the worker");
            running = true;
            worker = std::thread([this]() { worker_loop(); });
        }

        void ConfigService::stop_worker() {
            spdlog::info("stop the worker");
            running = false;
            if (worker.joinable()) {
                worker.join();
            }
        }

        void ConfigService::worker_loop() {
            int loops = 0;
            while (running) {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    loops++;
                    spdlog::info("read the config file: {}", loops);

                    std::ifstream fin(ctx.cfg_filename);
                    json data = json::parse(fin);
                    spdlog::info("{}", data.dump());
                }
                
                std::this_thread::sleep_for(ctx.sleep_duration);
            }
        }

        // Public interface implementations
        json web_config() {
            return ConfigService::instance().web_config();
        }

        json client_config(const std::string& client_name) {
            return ConfigService::instance().client_config(client_name);
        }

        void configure(const ServiceContext& config) {
            ConfigService::configure(config);
        }
    }
}
