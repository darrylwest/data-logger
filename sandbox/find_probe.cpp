#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp> // Include nlohmann/json for JSON parsing

using json = nlohmann::json;

struct Probe {
    int sensor;
    std::string location;
    bool enabled;
};

std::optional<Probe> find_probe(const json& data, const std::string& probe_location) {
    if (!data.is_object()) {
        return std::nullopt; // Invalid JSON format
    }

    if (data.contains("sensors") && data["sensors"].is_array()) {
        for (const auto& sensor_data : data["sensors"]) {
            if (sensor_data.contains("probes") && sensor_data["probes"].is_array()) {
                for (const auto& probe_data : sensor_data["probes"]) {
                     if (probe_data.is_object() && probe_data.contains("location") && probe_data["location"].is_string() && probe_data["location"] == probe_location) {
                        try {
                            Probe probe;
                            probe.sensor = probe_data.value("sensor", -1); // Provide a default value in case "sensor" is missing
                            probe.location = probe_data["location"].get<std::string>();
                            probe.enabled = probe_data.value("enabled", false); // Provide a default value

                            return probe;
                        } catch (const json::type_error& e) {
                            std::cerr << "Error parsing probe data: " << e.what() << std::endl;
                            return std::nullopt;
                        }

                    }
                }
            }
        }
    }

    return std::nullopt; // Probe not found
}

int main() {
    std::string json_data = R"({
        "location": "cottage",
        "ip": "10.0.1.197",
        "port": 2030,
        "active": true,
        "sensors": [
            {
                "type": "temperature",
                "path": "/temps",
                "probes": [
                    { "sensor": 0, "location": "cottage-south", "enabled": true },
                    { "sensor": 1, "location": "cottage-east", "enabled": false}
                ]
            }
        ]
    })";

    try {
        auto j = json::parse(json_data);

        auto probe = find_probe(j, "cottage-east");

        if (probe.has_value()) {
            std::cout << "Probe found:" << std::endl;
            std::cout << "Sensor: " << probe->sensor << std::endl;
            std::cout << "Location: " << probe->location << std::endl;
            std::cout << "Enabled: " << probe->enabled << std::endl;
        } else {
            std::cout << "Probe not found." << std::endl;
        }

       probe = find_probe(j, "non-existent-probe");
        if (probe.has_value()) {
            std::cout << "Probe found:" << std::endl;
            std::cout << "Sensor: " << probe->sensor << std::endl;
            std::cout << "Location: " << probe->location << std::endl;
            std::cout << "Enabled: " << probe->enabled << std::endl;
        } else {
            std::cout << "Probe not found." << std::endl;
        }

    } catch (const json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
