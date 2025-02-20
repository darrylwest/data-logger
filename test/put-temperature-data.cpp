#include <httplib.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>

unsigned int unix_ts() {
    using namespace std::chrono;
    auto now = system_clock::now();
    return duration_cast<seconds>(now.time_since_epoch()).count();
}

int main() {
    httplib::Client cli("http://localhost:9090");

    auto key = fmt::format("{}.tmp.0", unix_ts());
    nlohmann::json json_data = {
        {"key", key },
        {"value", 10.1234}  // No need to convert manually
    };

    std::string body = json_data.dump();  // Convert JSON to string
    spdlog::info("put data: {}", body);

    auto path = "/temperature";
    auto res = cli.Put(path, body, "application/json");

    if (res) { 
        if (res->status == 200) {
            spdlog::info("good response: {}, {}", res->status, res->body);
        } else {
            spdlog::error("bad response: {} {}", res->status, res->body);
        }
    } else {
        auto err = httplib::to_string(res.error());
        spdlog::error("request failed: {}", err);
    }

    return 0;
}

