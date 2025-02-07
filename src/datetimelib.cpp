//
// 2025-02-07 00:08:09 dpw
//

#include <chrono>
#include <app/types.hpp>
#include <app/datetimelib.hpp>


namespace datetimelib {
    // unix timestamp
    unsigned int timestamp_seconds() {
        using namespace std::chrono;
        auto now = system_clock::now();

        return duration_cast<seconds>(now.time_since_epoch()).count();
    }

    // get the timestamp in millis
    unsigned long timestamp_millis() {
        using namespace std::chrono;
        auto now = system_clock::now();
        return duration_cast<milliseconds>(now.time_since_epoch()).count();
    }

    // returns the local datetime in iso8601 format
    Str local_iso_datetime(const std::time_t now_seconds) {
        using namespace std::chrono;

        // const auto ts = timestamp_seconds();
        const time_t ts = (now_seconds == 0) ?  timestamp_seconds() : now_seconds;

        system_clock::time_point tp = system_clock::from_time_t(ts);
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);

        // convert to local time
        std::tm local_tm = *std::localtime(&tt);

        // format as iso8601
        std::ostringstream oss;
        oss << std::put_time(&local_tm, "%Y-%m-%dT%H:%M:%S%z");

        return oss.str();
    }
}
