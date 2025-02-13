//
// 2025-02-04 17:01:53 dpw
//

#include <algorithm>
#include <app/database.hpp>
#include <app/datetimelib.hpp>
#include <app/exceptions.hpp>
#include <app/types.hpp>
#include <chrono>
#include <iomanip>
// #include <nlohmann/json.hpp>

/*
 * create a k/v compatible with future redis integration
 */
namespace app {
    namespace database {
        // define reading key YYYY-MM-DDTHH:MM.TP.location.probe-location
        // define reading key YYYY-MM-DDTHH:MM.ST.location
        DbKey create_key(const Str& datetime, const Str& location) {
            auto dt = datetimelib::parse_datetime_to_minutes(datetime);
            return DbKey{.datetime = dt, .location = location};
        }

        // append the key/value to the file; throws on error; returns the number of bytes written
        void append_key_value(const Str& filename, const DbKey& key, const Str& value) {
            std::ofstream file(filename, std::ios::app);

            if (!file.is_open()) {
                const auto msg
                    = "Error in database::append_key_value(); can't open file: " + filename;
                spdlog::error(msg);
                throw app::FileException(msg);
            }

            file << key << "=" << value << std::endl;
            file.close();
        }

        bool read_current_data(Database& db) {
            Str location = "deck-west";
            Str filename = "data/temperature/current." + location + ".db";

            spdlog::info("read current data from {}", filename);
            db.read(filename, false);
            spdlog::info("db size: {}", db.size());

            return true;
        }

        bool Database::set(const Str& key, const Str& value) {
            std::lock_guard<std::mutex> lock(mtx);
            data[key] = value;
            return true;
        }

        Str Database::get(const Str& key) const {
            std::lock_guard<std::mutex> lock(mtx);
            auto it = data.find(key);
            if (it != data.end()) {
                return it->second;
            }
            return "";  // Return an empty string if key is not found
        }

        // Thread-safe keys method with optional filter
        Vec<Str> Database::keys(const Str& search) const {
            std::lock_guard<std::mutex> lock(mtx);
            Vec<Str> key_list;
            for (const auto& [key, _] : data) {
                if (search.empty() || key.find(search) != Str::npos) {
                    key_list.push_back(key);
                }
            }
            return key_list;
        }

        size_t Database::size() const { return data.size(); }

        // Thread-safe read from file
        bool Database::read(const Str& filename, bool clear) {
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
        bool Database::save(const Str& filename) const {
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
    }  // namespace database

}  // namespace app

/*
 * reading from client/node

2025-02-04T09:12:58, ts: 1738689178, sensor: 0, location: deck-west, tempC: 12.1719, tempF: 53.9094

 * raw readings from remote client/nodes

{"reading_at":{"time":"2025-02-04T09:04:37","ts":1738688677},"probes":[{"sensor":0,"location":"deck-west","millis":675764124,"tempC":12.09896,"tempF":53.77812}]}
{"reading_at":{"time":"2025-02-04T09:05:37","ts":1738688737},"probes":[{"sensor":0,"location":"deck-west","millis":675824124,"tempC":12.11979,"tempF":53.81562}]}
{"reading_at":{"time":"2025-02-04T09:06:37","ts":1738688797},"probes":[{"sensor":0,"location":"deck-west","millis":675884124,"tempC":12.125,"tempF":53.825}]}
{"reading_at":{"time":"2025-02-04T09:07:38","ts":1738688858},"probes":[{"sensor":0,"location":"deck-west","millis":675944124,"tempC":12.10417,"tempF":53.7875}]}
{"reading_at":{"time":"2025-02-04T09:08:38","ts":1738688918},"probes":[{"sensor":0,"location":"deck-west","millis":676004124,"tempC":12.10938,"tempF":53.79688}]}

* create the web/UI json structure
Str
    json j;

    // Add labels
    j["labels"]
        = {"09:30", "10:00", "10:30", "11:00", "11:30", "12:00", "12:30", "13:00", "13:30",
            "14:00", "14:30", "15:00", "15:30", "16:00", "16:30", "17:00", "17:30", "18:00",
            "18:30", "19:00", "19:30", "20:00", "20:30", "21:00", "21:30"};

    // Sensor 1 data
    json sensor1
        = {{"sensor_id", "sensor_1"},
            {"label", "cottage-south"},
            {"data", {49.5781,  50.2531,  50.8156,  50.7406,  51.5281,  51.8094,  51.9688,
                        51.9969,  52.3906,  52.5312,  52.5219,  53.6375,  52.7,     52.82187,
                        52.82187, 49.71875, 47.75937, 46.46563, 45.94062, 44.51563, 44.51563,
                        43.95313, 43.95313, 43.86875, 43.86875}},
            {"borderColor", "red"},
            {"fill", false}};

    // Sensor 2 data
    json sensor2 = {{"sensor_id", "sensor_2"},
                    {"label", "shed-west"},
                    {"data", {49.32900,  49.521116, 50.02136,  51.18444,  52.293116,
                                51.43887,  52.48388,  52.980233, 53.10152,  52.00872,
                                52.635993, 54.16272,  52.29799,  53.557073, 51.98579,
                                48.73718,  48.225931, 45.70019,  45.66436,  44.396992,
                                44.27969,  43.71025,  43.768674, 44.14863,  43.34765}},
                    {"borderColor", "blue"},
                    {"fill", false}};

    // Add datasets and end_date
    j["datasets"] = {sensor1, sensor2};
    j["end_date"] = "2025-01-30T21:00";

    return j.dump();
};
*/
