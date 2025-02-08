//
// 2025-02-08 14:47:10 dpx
// g++ -std=c++20 -Wall -o json-config json-config.cpp
//

#include <spdlog/fmt/fmt.h>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

struct Client {
    std::string location;
    std::string ip;
    int port;
    bool active;
};

std::string get_web_url(json ws) {
    int port = ws["port"].template get<int>();
    return fmt::format("{}://{}:{}", ws["scheme"], ws["host"], port);
}

std::string get_client_url(json client) {
    int port = client["port"].template get<int>();
    return fmt::format("http://{}:{}", client["ip"], port);
}

Client parse_client(json cfg, std::string location) {
    Client cli;
    for (const auto& client : cfg["clients"]) {
        if (client["location"] == location) {
            cli.location = location;
            cli.ip = client["ip"];
            cli.port = client["port"].template get<int>();
            cli.active = client["active"].template get<bool>();
            return cli;
        }
    }

    return cli;
    // throw app::ParseException("client not found for location: " + location);
}

// Example usage (you'd typically populate this from a JSON library)
int main() {
    std::ifstream fin("../config/config.json");
    json data = json::parse(fin);

    std::cout << "version: " << data["version"] << std::endl;

    if (false) {
        std::cout << data.dump(2) << std::endl;

        std::cout << "webservice : " << data["webservice"] << std::endl;
        std::cout << "web url: " << get_web_url(data["webservice"]) << std::endl;
    }

    json clients = data["clients"];
    std::cout << "clients: " << clients.dump() << std::endl;

    auto client = parse_client(data, "shedr");
    auto active = client.active ? "yes" : "no";
    std::cout << "shed client ip: " << client.ip
        << ":" << client.port 
        << " active: " << active
        << std::endl;

    return 0;
}
