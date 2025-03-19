//
// 2025-02-21 02:51:00 dpw
//

#pragma once

#include <app/types.hpp>
#include <nlohmann/json.hpp>

#include "precompiled.hpp"

namespace app::cfgsvc {
    using json = nlohmann::json;

    struct ServiceContext {
        Str cfg_filename = "config/config.json";
        std::chrono::seconds sleep_duration{10};  // Sleep time between work
        Func<Vec<Str>(const json&)> validate = [](const json&) { return Vec<Str>(); };
        Str json_text = {};
    };

    // TODO add typical use to show how to set the context and init/configure
    class ConfigService {
      public:
        static ConfigService& instance();

        // configure and start (or restart) the service with the context
        static void configure(const ServiceContext& ctx);

        // return true if the service is running, else false
        bool is_running();

        json get_node(const StrView& node_name);

        json webservice();
        json clients();
        json client(const std::string& client_name);

        // TODO add methods to return known objects rather than json

        ~ConfigService();

        // delete the copy constructor
        ConfigService(const ConfigService&) = delete;

        // delete the copy operator
        ConfigService& operator=(const ConfigService&) = delete;

      private:
        ConfigService();

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

    json get_node(const StrView& node_name);

    json webservice();
    json clients();
    json client(const Str& client_name);

    void configure(const ServiceContext& config);
    bool is_running();
}  // namespace app::cfgsvc
