//
// 2025-02-21 02:51:00 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/types.hpp>
#include <app/cfgsvc.hpp>
#include <app/exceptions.hpp>
#include <app/jsonkeys.hpp>
#include <fstream>

namespace app {
    namespace cfgsvc {
        using json = nlohmann::json;
        using namespace app::jsonkeys;

        ConfigService& ConfigService::instance() {
            static ConfigService service;
            return service;
        }

        template <typename T>
        T ConfigService::get(const std::function<T(const json&)>& func) {
            std::lock_guard<std::mutex> lock(mtx);

            try {
                return func(app_config);  // Execute the provided function
            } catch (const std::exception& e) {
                throw std::runtime_error(fmt::format("Failed to execute function: {}", e.what()));
            }
        }

        json ConfigService::web_config() {
            std::lock_guard<std::mutex> lock(mtx);

            json j = instance().app_config[WEBSERVICE];
            spdlog::info("return the web config: {}", j.dump());

            return j;
        }

        json ConfigService::client_config(const std::string& client_name) {
            spdlog::info("return the client config for {}", client_name);
            std::lock_guard<std::mutex> lock(mtx);
            json j = instance().app_config[CLIENTS][0];

            return j;
        }

        bool ConfigService::is_running() { return instance().running.load(); }

        void ConfigService::configure(const ServiceContext& ctx) {
            spdlog::info("configure and start the config service");
            auto& service = instance();

            try {
                service.load_config();
            } catch (const std::exception& e) {
                spdlog::error("error reading config file: {} {}", ctx.cfg_filename, e.what());
                throw e;
            }

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

        void ConfigService::load_config() {
            std::ifstream fin(ctx.cfg_filename);
            if (!fin) {
                throw app::FileException("Failed to open config file: " + ctx.cfg_filename);
            }

            json data;
            try {
                data = json::parse(fin);
            } catch (const json::parse_error& e) {
                Str msg
                    = fmt::format("JSON parse error on config: {}, {}", ctx.cfg_filename, e.what());
                spdlog::error(msg);
                throw app::ParseException(msg);
            }

            std::lock_guard<std::mutex> lock(mtx);
            app_config = std::move(data);  // Update app_config
            spdlog::info("updated app_config: {}", app_config.dump());
        }

        void ConfigService::worker_loop() {
            while (running) {
                std::this_thread::sleep_for(ctx.sleep_duration);
                try {
                    spdlog::info("read the config file: {}", ctx.cfg_filename);
                    load_config();
                } catch (const std::exception& e) {
                    spdlog::error("config loop has been stopped: {} {}", ctx.cfg_filename,
                                  e.what());
                }
            }

            spdlog::warn("worker thread is exiting, running = {}", running.load());
        }

        // Public interface implementations

        template <typename T>
        T get(const Func<T(const json&)>& func) {
             return ConfigService::instance().get<json>(func);
        }

        json web_config() { return ConfigService::instance().web_config(); }

        json client_config(const std::string& client_name) {
            return ConfigService::instance().client_config(client_name);
        }

        void configure(const ServiceContext& config) { ConfigService::configure(config); }

        bool is_running() { return ConfigService::instance().is_running(); }
    }  // namespace cfgsvc
}  // namespace app
