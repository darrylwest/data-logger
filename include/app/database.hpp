//
// 2025-01-10 10:28:16 dpw
//

#pragma once

#include <spdlog/spdlog.h>

#include <app/types.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
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
            static Str to_label(Value v) {
                switch (v) {
                    case Value::Status:
                        return "status";
                    case Value::Temperature:
                        return "temperature";
                    case Value::Light:
                        return "light";
                    case Value::Humidity:
                        return "humidity";
                    case Value::Proximity:
                        return "proximity";
                    case Value::Distance:
                        return "distance";
                    default:
                        return "unknown";
                }
            }

            // return the numeric value
            static short to_value(Value reading_type) { return static_cast<short>(reading_type); }
        };

        struct DbKey {
            Str datetime;
            Str location;

            friend std::ostream& operator<<(std::ostream& os, const DbKey v) {
                os << v.datetime << "." << v.location;

                return os;
            }

            Str to_string() const {
                std::ostringstream oss;
                oss << *this;

                return oss.str();
            }
        };

        // create the db key from iso8601 datetime string and the probe location
        const DbKey create_key(const Str datetime, const Str location);

        // parse the datetimme string (iso8601) to a 12 character yyyymmddhhmm
        const Str parse_datetime(const Str& datetime);

        // truncate an iso date to the nearest n minutes, defaulting to 5 minutes
        const Str truncate_to_minutes(const Str& isodate, const int minute = 5);

        // append the key/value to the file; throws FileException on error
        const void append_key_value(const Str& filename, const DbKey& key, const Str& value);

        // get the current local time from the timestamp
        const Str timestamp_to_local(const std::time_t timestamp);

        struct Database {
          private:
            std::map<Str, Str> data;
            mutable std::mutex mtx;  // mutable to allow locking in const methods

          public:
            // Thread-safe set method
            bool set(const Str& key, const Str& value);

            // Thread-safe get method
            Str get(const Str& key) const;

            // Thread-safe remove method
            bool remove(const Str& key) {
                std::lock_guard<std::mutex> lock(mtx);
                return data.erase(key) > 0;
            }

            // Thread-safe keys method with optional filter
            Vec<Str> keys(const Str& search = "") const;

            // return the current database size
            size_t size() const;

            // Thread-safe read from file
            bool read(const Str& filename, bool clear = false) {
                std::lock_guard<std::mutex> lock(mtx);
                std::ifstream infile(filename);
                if (!infile.is_open()) {
                    return false;
                }

                if (clear) {
                    spdlog::info("clearing the database prior to read");
                    data.clear();
                }

                Str line;
                while (std::getline(infile, line)) {
                    std::istringstream iss(line);
                    Str key, value;
                    if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                        data[key] = value;
                    }
                }
                return true;
            }

            // Thread-safe dump/save to file
            bool save(const Str& filename) const {
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
        };  // database

    }  // namespace database
}  // namespace app
