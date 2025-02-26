#include <httplib.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

int main() {
    httplib::Server svr;

    svr.Put("/temperature", [&](const httplib::Request &req, httplib::Response &res) {
        try {
            auto json_body = nlohmann::json::parse(req.body);
            if (json_body.contains("key") && json_body.contains("value")) {
                std::string key = json_body["key"].get<std::string>();
                float value = json_body["value"].get<float>();

                spdlog::info("put key: {}, value: {}", key, value);

                res.set_content("ok", "text/plain");
            } else {
                res.status = 400;
                res.set_content("Missing key or value", "text/plain");
            }
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content("Invalid JSON format", "text/plain");
        }
    });

    std::cout << "Server running on port 8888..." << std::endl;
    svr.listen("0.0.0.0", 8888);

    return 0;
}

