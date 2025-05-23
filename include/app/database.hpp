//
// 2025-01-10 10:28:16 dpw
//

#pragma once

#include <app/types.hpp>

#include "precompiled.hpp"

namespace app::database {
    using FilterFunc = std::function<bool(const Str&)>;
    using SortedMap = std::map<Str, Str>;

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
            }

            throw std::invalid_argument("unknown value type");
        }

        // return the numeric value
        static short to_value(Value reading_type) { return static_cast<short>(reading_type); }
    };

    struct DbKey {
        std::time_t timestamp;
        Str location;

        friend std::ostream& operator<<(std::ostream& os, const DbKey v) {
            os << v.timestamp << "." << v.location;

            return os;
        }

        [[nodiscard]] auto to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };

    // create the db key from unix timestamp and the probe location
    DbKey create_key(const time_t& timestamp, const Str& location);

    // append the key/value to the file; throws FileException on error
    void append_key_value(const FilePath& filename, const DbKey& key, const Str& value);

    // a lambda to pass to Database::keys() (the default)
    static FilterFunc all_keys = [](const Str&) { return true; };

    struct Database {
      private:
        std::map<Str, Str> data;
        mutable std::mutex mtx;  // mutable to allow locking in const methods

      public:
        // Thread-safe set method; inserts or updates the key/value pair; true if inserted
        bool set(const Str& key, const Str& value);

        // Thread-safe get method; returns data for key or an empty string if not found
        // TODO replace the return with a std::expected rather than an empty string
        Optional<Str> get(const Str& key) const;

        // Thread-safe remove method
        bool remove(const Str& key) {
            std::lock_guard<std::mutex> lock(mtx);
            return data.erase(key) > 0;
        }

        // return the last n number of k/v elements
        SortedMap last(size_t count = 1) const;

        // Thread-safe keys method with optional filter function
        Vec<Str> keys(const FilterFunc& filter = all_keys) const;

        // search with FilterFunc returns a SortedMap
        SortedMap search(const FilterFunc& filter = all_keys) const;

        // return the current database size
        size_t size() const;

        // read from file to populate database; optionally clear the db first
        bool read(const FilePath& path, bool clear = false);

        // save the current database to file
        bool write(const FilePath& path) const;

    };  // struct database

}  // namespace app::database
