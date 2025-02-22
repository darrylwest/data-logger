//
// 2025-02-21 02:51:00 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cfgsvc.hpp>
#include <app/exceptions.hpp>
#include <app/jsonkeys.hpp>
#include <app/types.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace app {
    namespace cfgsvc {
        using json = nlohmann::json;
        using namespace app::jsonkeys;

        ConfigService& ConfigService::instance() {
            static ConfigService service;
            return service;
        }

        json ConfigService::webservice() {
            std::lock_guard<std::mutex> lock(mtx);

            json j = instance().app_config[WEBSERVICE];
            spdlog::info("return the web config: {}", j.dump());

            return j;
        }

        json ConfigService::client(const Str& client_name) {
            spdlog::info("return the client config for {}", client_name);
            std::lock_guard<std::mutex> lock(mtx);
            json jclients = instance().app_config[CLIENTS];

            for (const auto& jclient : jclients) {
                if (jclient[LOCATION] == client_name) {
                    return jclient;
                }
            }

            throw std::runtime_error("can not find client named: " + client_name);
        }

        json ConfigService::clients() {
            spdlog::info("return all clients in the list");

            std::lock_guard<std::mutex> lock(mtx);
            json j = instance().app_config[CLIENTS];

            return j;
        }

        bool ConfigService::is_running() { return instance().running.load(); }

        void ConfigService::configure(const ServiceContext& new_ctx) {
            spdlog::info("configure, validate, and start the config service");
            auto& service = instance();

            try {
                service.load_config();
            } catch (const std::exception& e) {
                spdlog::error("Error reading config file: {}", new_ctx.cfg_filename);
                throw;  // re-throw the original exception
            }

            std::lock_guard<std::mutex> lock(service.mtx);
            service.ctx = new_ctx;  // Only assign if validation passed

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

        // read in, parse and validate the json config
        void ConfigService::load_config() {
            std::ifstream fin(ctx.cfg_filename);
            if (!fin) {
                throw app::FileException("Failed to open config file: " + ctx.cfg_filename);
            }

            json data;
            try {
                data = json::parse(fin);
            } catch (const json::parse_error& e) {
                Str msg = fmt::format("JSON parse error on config: {}, {}", ctx.cfg_filename, e.what());
                spdlog::error(msg);
                throw app::ParseException(msg);
            }

            // Run validation on `app_config`
            Vec<Str> errors = ctx.validate(data);
            if (!errors.empty()) {
                for (const auto& err : errors) {
                    spdlog::error("Validation error: {}", err);
                }
                const auto msg = fmt::format("config not valid for file: {}", ctx.cfg_filename);
                spdlog::error(msg);
                throw ValidationException(msg);
            }

            std::lock_guard<std::mutex> lock(mtx);
            app_config = std::move(data);  // Update app_config
            spdlog::info("updated app_config: {}", app_config.dump());
        }

        void ConfigService::worker_loop() {
            namespace fs = std::filesystem;
            fs::file_time_type last_read_time;

            while (running) {
                // sleep first because the file was read/validated on configure
                std::this_thread::sleep_for(ctx.sleep_duration);

                try {
                    if (!fs::exists(ctx.cfg_filename)) {
                        spdlog::warn("Config file does not exist: {}", ctx.cfg_filename);
                        continue;  // Skip this iteration if file is missing
                    }

                    fs::file_time_type current_mod_time = fs::last_write_time(ctx.cfg_filename);

                    if (current_mod_time > last_read_time) {  // Only reload if modified
                        spdlog::info("Config file updated, reloading: {}", ctx.cfg_filename);
                        load_config();
                        last_read_time = current_mod_time;  // Update last read time
                    } else {
                        spdlog::info("Config file unchanged, skipping reload.");
                    }
                } catch (const std::exception& e) {
                    spdlog::error("config loop error: {} {}", ctx.cfg_filename, e.what());
                }
            }

            spdlog::warn("worker thread is exiting, running = {}", running.load());
        }

        // Public interface implementations

        json webservice() { return ConfigService::instance().webservice(); }

        json clients() { return ConfigService::instance().clients(); }

        json client(const std::string& client_name) { return ConfigService::instance().client(client_name); }

        void configure(const ServiceContext& config) { ConfigService::configure(config); }

        bool is_running() { return ConfigService::instance().is_running(); }
    }  // namespace cfgsvc
}  // namespace app
