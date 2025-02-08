//
// 2025-02-08 14:47:10 dpx
// g++ -std=c++20 -Wall -o json-config json-config.cpp
//

#include <spdlog/fmt/fmt.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <variant>

namespace config {


struct Header {
    std::string key;
    std::string value;
};

struct Webservice {
    std::string scheme;
    std::string host;
    int port;
    std::vector<Header> headers;

    static std::string to_string(Webservice ws) {
        return fmt::format("{}://{}:{}", ws.scheme, ws.host, ws.port);
    }
};

struct Client {
    std::string location;
    std::string ip;
    int port;
    bool active;
};

struct Probe {
    int sensor;
    std::string location;
};

struct TemperatureLocation {
    std::string client;
    std::vector<Probe> probes;
};

struct Temperature {
    std::vector<TemperatureLocation> locations;
};

struct Config {
    std::string version = "1.0.0-100";
    Webservice webservice;
    std::vector<Client> clients;
    Temperature temperature;
};

} // namespace config


// Example usage (you'd typically populate this from a JSON library)
int main() {
    config::Config cfg;

    cfg.webservice.scheme = "http";
    cfg.webservice.host = "0.0.0.0";
    cfg.webservice.port = 9090;
    cfg.webservice.headers.push_back(config::Header{.key = "x-thing", .value = "flarb"});

    cfg.clients.push_back({"cottage", "10.0.1.197", 2030, true});
    cfg.clients.push_back({"shed", "10.0.1.115", 2030, false});

    cfg.temperature.locations.push_back({"cottage", {{0, "cottage-south"}, {1, "cottage-east"}}});
    cfg.temperature.locations.push_back({"shed", {{0, "shed-inside"}, {1, "shed-west"}}});


    std::cout << "version: " << cfg.version << "\n";
    std::cout << "service: " << config::Webservice::to_string(cfg.webservice) << std::endl;

    
    return 0;
}
