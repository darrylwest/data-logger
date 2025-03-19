//
// 2025-02-04 17:01:53 dpw
//

#include <app/database.hpp>
#include <app/exceptions.hpp>
#include <app/types.hpp>
#include <datetimelib/datetimelib.hpp>

#include "precompiled.hpp"

/*
 * create a k/v compatible with future redis integration
 */
namespace app {
    namespace database {
        // define reading key YYYY-MM-DDTHH:MM.TP.location.probe-location
        // define reading key YYYY-MM-DDTHH:MM.ST.location
        DbKey create_key(const time_t& timestamp, const Str& location) {
            return DbKey{.timestamp = timestamp, .location = location};
        }

        // append the key/value to the file; throws on error; returns the number of bytes written
        void append_key_value(const FilePath& path, const DbKey& key, const Str& value) {
            std::ofstream file(path, std::ios::app);

            if (!file.is_open()) {
                const auto msg
                    = "Error in database::append_key_value(); can't open file: " + path.string();
                spdlog::error(msg);
                throw app::FileException(msg);
            }

            file << key << "=" << value << '\n';
            file.close();
        }

        bool read_current_data(Database& db) {
            // TODO read from configuration, all database files
            Str location = "cottage";
            const FilePath path = "data/temperature/current." + location + ".db";

            spdlog::info("read current data from {}", path.string());
            db.read(path, false);
            spdlog::info("db size: {}", db.size());

            return true;
        }

        bool Database::set(const Str& key, const Str& value) {
            std::lock_guard<std::mutex> lock(mtx);
            if (data.contains(key)) {
                data[key] = value;
                return false;
            } else {
                data.emplace_hint(data.end(), key, value);
                return true;
            }
        }

        Optional<Str> Database::get(const Str& key) const {
            std::lock_guard<std::mutex> lock(mtx);
            auto it = data.find(key);
            if (it != data.end()) {
                return it->second;
            } else {
                return std::nullopt;
            }
        }

        SortedMap Database::last(const size_t count) const {
            SortedMap result;
            std::lock_guard<std::mutex> lock(mtx);

            if (count <= 0 || data.empty()) return result;  // Handle edge cases

            // Create a view that skips the first (size - count) elements
            auto last_view = data | std::views::drop(data.size() > count ? data.size() - count : 0);

            // Copy selected elements into result (still sorted)
            std::ranges::copy(last_view, std::inserter(result, result.end()));

            return result;  // Already in sorted order
        }

        // Thread-safe keys method with optional filter; returns sorted vector
        // returns a new Vec<Str> as a copy.
        Vec<Str> Database::keys(const FilterFunc& filter) const {
            Vec<Str> key_list;

            std::lock_guard<std::mutex> lock(mtx);

            // extract keys into views::keys then filter
            std::ranges::copy_if(data | std::views::keys, std::back_inserter(key_list), filter);

            return key_list;
        }

        SortedMap Database::search(const FilterFunc& filter) const {
            SortedMap map;

            std::lock_guard<std::mutex> lock(mtx);

            std::ranges::copy_if(data, std::inserter(map, map.end()),
                                 [&](const auto& pair) { return filter(pair.first); });

            return map;
        }

        size_t Database::size() const { return data.size(); }

        // Thread-safe read from file
        bool Database::read(const FilePath& path, bool clear) {
            std::lock_guard<std::mutex> lock(mtx);
            std::ifstream infile(path);
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
        bool Database::save(const FilePath& path) const {
            std::lock_guard<std::mutex> lock(mtx);
            std::ofstream outfile(path);
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
