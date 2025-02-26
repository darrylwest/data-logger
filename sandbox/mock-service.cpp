//
// g++ -std=c++20 -o json_example json_example.cpp
//
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    json j;

    // Add labels
    j["labels"] = {
        "09:30", "10:00", "10:30", "11:00", "11:30",
        "12:00", "12:30", "13:00", "13:30", "14:00",
        "14:30", "15:00", "15:30", "16:00", "16:30",
        "17:00", "17:30", "18:00", "18:30", "19:00",
        "19:30", "20:00", "20:30", "21:00", "21:30"
    };

    // Sensor 1 data
    json sensor1 = {
        {"sensor_id", "sensor_1"},
        {"label", "cottage-south"},
        {"data", {
            49.5781, 50.2531, 50.8156, 50.7406, 51.5281,
            51.8094, 51.9688, 51.9969, 52.3906, 52.5312,
            52.5219, 53.6375, 52.7, 52.82187, 52.82187,
            49.71875, 47.75937, 46.46563, 45.94062, 44.51563,
            44.51563, 43.95313, 43.95313, 43.86875, 43.86875
        }},
        {"borderColor", "red"},
        {"fill", false}
    };

    // Sensor 2 data
    json sensor2 = {
        {"sensor_id", "sensor_2"},
        {"label", "shed-west"},
        {"data", {
            49.32900, 49.521116, 50.02136,
            51.18444, 52.293116, 51.43887,
            52.48388, 52.980233, 53.10152,
            52.00872, 52.635993, 54.16272,
            52.29799, 53.557073, 51.98579,
            48.73718, 48.225931, 45.70019,
            45.66436, 44.396992, 44.27969,
            43.71025, 43.768674, 44.14863,
            43.34765
        }},
        {"borderColor", "blue"},
        {"fill", false}
    };

    // Add datasets and end_date
    j["datasets"] = {sensor1, sensor2};
    j["end_date"] = "2025-01-30T21:00";

    // Output the JSON
    std::cout << j.dump() << std::endl;

    return 0;
}

