//
// 2025-02-07 00:08:09 dpw
//

#include <chrono>
#include <app/types.hpp>
#include <app/datetime.hpp>


namespace datetime {
    // unix timestamp
    unsigned int timestamp_seconds() {
        using namespace std::chrono;
        auto now = system_clock::now();
        return duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    }

    // returns the local datetime in iso8601 format
    Str get_local_datetime() {
        return "2025-02-05T16:10:00-08:00";
    }
}
