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

std::string get_web_url(json ws) {
    int port = ws["port"].template get<int>();
    return fmt::format("{}://{}:{}", ws["scheme"], ws["host"], port);
}

std::string get_client_url(json client) {
    int port = client["port"].template get<int>();
    return fmt::format("http://{}:{}", client["ip"], port);
}

// Example usage (you'd typically populate this from a JSON library)
int main() {
    std::ifstream fin("../config/config.json");
    json data = json::parse(fin);

    std::cout << data.dump(2) << std::endl;

    std::cout << "version: " << data["version"] << std::endl;
    std::cout << "webservice : " << data["webservice"] << std::endl;
    std::cout << "web url: " << get_web_url(data["webservice"]) << std::endl;

    json clients = data["clients"];
    std::cout << "clients: " << clients.dump() << std::endl;

    for (const auto& client : clients) {
        std::cout << "client: " << client.dump() << std::endl;
        std::cout << "   url: " << get_client_url(client) << std::endl;
    }

    return 0;
}
