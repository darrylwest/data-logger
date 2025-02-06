//
// 2025-02-04 17:01:53 dpw
//

#include <algorithm>
#include <app/database.hpp>
#include <app/exceptions.hpp>
#include <chrono>
#include <iomanip>
#include <nlohmann/json.hpp>

/*
 * create a k/v compatible with future redis integration
 */
namespace app {
    namespace database {
        // define reading key YYYY-MM-DDTHH:MM.TP.location.probe-location
        // define reading key YYYY-MM-DDTHH:MM.ST.location
        const DbKey create_key(const std::string datetime, const std::string location) {
            auto dt = parse_datetime(datetime);
            return DbKey{.datetime = dt, .location = location};
        }

        // parse the datetimme string (iso8601) to a 12 character yyyymmddhhmm
        const std::string parse_datetime(const std::string& datetime) {
            std::string result;
            result.reserve(12);  // Reserve space for "YYYYMMDDHHMM"

            // Copy only digits
            std::copy_if(datetime.begin(), datetime.end(), std::back_inserter(result), ::isdigit);

            // Truncate to first 12 characters (YYYYMMDDHHMM)
            if (result.size() > 12) {
                result.resize(12);
            }

            return result;
        }

        // truncate the iso8601 date to the nearest minute, default 5 minute mark
        const std::string truncate_to_minutes(const std::string& isodate, const int minute) {
            // Parse the ISO 8601 datetime string
            std::tm tm = {};
            std::istringstream ss(isodate);
            ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

            // Convert to time_point
            auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));

            // Truncate to nearest 5 minutes
            auto minutes
                = std::chrono::duration_cast<std::chrono::minutes>(timePoint.time_since_epoch())
                  % minute;
            timePoint -= minutes;

            // Convert back to time_t
            std::time_t truncatedTime = std::chrono::system_clock::to_time_t(timePoint);
            std::tm* truncatedTm = std::localtime(&truncatedTime);

            // Format the result back to ISO 8601 without seconds
            std::ostringstream result;
            result << std::put_time(truncatedTm, "%Y-%m-%dT%H:%M");
            return result.str();
        }

        // append the key/value to the file; throws on error; returns the number of bytes written
        const void append_key_value(const std::string& filename, const DbKey& key,
                              const std::string& value) {
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

        // get the current local time from the timestamp
        const std::string timestamp_to_local(const std::time_t timestamp) {
            using namespace std::chrono;
            system_clock::time_point tp = system_clock::from_time_t(timestamp);

            std::time_t tt = system_clock::to_time_t(tp);

            std::tm local_tm = *std::localtime(&tt);

            std::ostringstream oss;

            oss << std::put_time(&local_tm, "%Y-%m-%dT%H:%M:%S%z");

            return oss.str();
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
std::string createFakeTemps() {
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
