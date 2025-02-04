//
// 2025-01-10 10:28:16 dpw
//

#pragma once

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace app {
    namespace database {
        using json = nlohmann::json;

        struct ReadingType {
            enum class Value : short {
                Status,
                Temperature,
                Light,
                Humidity,
                Proximity,
                Distance,
            };

            // return the label
            static std::string to_label(Value v) {
                switch (v) {
                    case Value::Status:
                        return "Status";
                    case Value::Temperature:
                        return "Temperature";
                    case Value::Light:
                        return "Light";
                    case Value::Humidity:
                        return "Humidity";
                    case Value::Proximity:
                        return "Proximity";
                    case Value::Distance:
                        return "Distance";
                    default:
                        return "Unknown";
                }
            }

            // return the numeric value
            static short to_value(Value reading_type) { return static_cast<short>(reading_type); }
        };

        struct DbKey {
            std::string datetime;
            ReadingType::Value type;
            std::string location;
        };

        // create the db key from iso8601 datetime string, the reading type and location
        DbKey create_key(std::string datetime, ReadingType::Value type, std::string location);

        // parse the datetimme string (iso8601) to a 12 character yyyymmddhhmm
        std::string parse_datetime(const std::string& datetime);

        struct Database {};
    }  // namespace database
}  // namespace app
