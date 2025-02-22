//
// 2025-02-21 02:51:00 dpw
//

#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <app/types.hpp>

namespace app {
    namespace cfgsvc {
        using json = nlohmann::json;

        struct ServiceContext {
            Str cfg_filename = "config/config.json";
            std::chrono::seconds sleep_duration{10};  // Sleep time between work
        };

        class ConfigService {
          public:
            static ConfigService& instance();

            // configure and start (or restart) the service
            static void configure(const ServiceContext& ctx);

            // return true if the service is running, else false
            bool is_running();

            template <typename T = json>
            T get(const Func<T(const json&)>& func);

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
        template <typename T = json>
        T get(const Func<T(const json&)>& func);

        json web_config();
        json client_config(const Str& client_name);
        void configure(const ServiceContext& config);
        bool is_running();
    }  // namespace cfgsvc
}  // namespace app
