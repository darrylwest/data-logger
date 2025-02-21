//
// 2025-02-21 02:51:00 dpw
//

#pragma once

#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <nlohmann/json.hpp>

namespace app {
    namespace cfgsvc {
        using json = nlohmann::json;

        struct ServiceContext {
            std::string cfg_filename = "config/config.json";
            std::chrono::seconds sleep_duration{10}; // Sleep time between work
        };

        class ConfigService {
        public:
            static ConfigService& instance();

            // configure and start (or restart) the service
            static void configure(const ServiceContext& ctx);

            // return true if the service is running, else false
            bool is_running();

            json web_config();
            json client_config(const std::string& client_name);

            ~ConfigService();

        private:
            ConfigService();
            ConfigService(const ConfigService&) = delete;
            ConfigService& operator=(const ConfigService&) = delete;

            void start_worker();
            void stop_worker();
            void worker_loop();
            void load_config();

            json app_config;
            ServiceContext ctx;
            std::mutex mtx;
            std::thread worker;
            std::atomic<bool> running{false};
        };

        // Public interface functions
        json web_config();
        json client_config(const std::string& client_name);
        void configure(const ServiceContext& config);
        bool is_running();
    }
}

