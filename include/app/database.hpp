//
// 2025-01-10 10:28:16 dpw
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <optional>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <sstream>

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

            friend std::ostream& operator<<(std::ostream& os, const DbKey v) {
                os << v.datetime << ReadingType::to_value(v.type) << "." << v.location;

                return os;
            }

            std::string to_string() const {
                std::ostringstream oss;
                oss << *this;

                return oss.str();
            }
        };

        // create the db key from iso8601 datetime string, the reading type and location
        DbKey create_key(std::string datetime, ReadingType::Value type, std::string location);

        // parse the datetimme string (iso8601) to a 12 character yyyymmddhhmm
        std::string parse_datetime(const std::string& datetime);

        struct Database {
        private:
            std::map<std::string, std::string> data;
            mutable std::mutex mtx; // mutable to allow locking in const methods

        public:
            // Thread-safe set method
            bool set(const std::string& key, const std::string& value) {
                std::lock_guard<std::mutex> lock(mtx);
                data[key] = value;
                return true;
            }

            // Thread-safe get method
            std::string get(const std::string& key) const {
                std::lock_guard<std::mutex> lock(mtx);
                auto it = data.find(key);
                if (it != data.end()) {
                    return it->second;
                }
                return ""; // Return an empty string if key is not found
            }

            // Thread-safe remove method
            bool remove(const std::string& key) {
                std::lock_guard<std::mutex> lock(mtx);
                return data.erase(key) > 0;
            }

            // Thread-safe keys method with optional filter
            std::vector<std::string> keys(const std::string& search = "") const {
                std::lock_guard<std::mutex> lock(mtx);
                std::vector<std::string> key_list;
                for (const auto& [key, _] : data) {
                    if (search.empty() || key.find(search) != std::string::npos) {
                        key_list.push_back(key);
                    }
                }
                return key_list;
            }

            // return the current database size
            size_t size() {
                return data.size();
            }

            // Thread-safe read from file
            bool read(const std::string& filename, bool clear = false) {
                std::lock_guard<std::mutex> lock(mtx);
                std::ifstream infile(filename);
                if (!infile.is_open()) {
                    return false;
                }

                if (clear) {
                    spdlog::info("clearing the database prior to read");
                    data.clear();
                }

                std::string line;
                while (std::getline(infile, line)) {
                    std::istringstream iss(line);
                    std::string key, value;
                    if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                        data[key] = value;
                    }
                }
                return true;
            }

            // Thread-safe dump/save to file
            bool save(const std::string& filename) const {
                std::lock_guard<std::mutex> lock(mtx);
                std::ofstream outfile(filename);
                if (!outfile.is_open()) {
                    return false;
                }
                for (const auto& [key, value] : data) {
                    outfile << key << "=" << value << "\n";
                }
                return true;
            }
        }; // database

    }  // namespace database
}  // namespace app
