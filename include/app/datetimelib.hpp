//
// 2025-02-07 00:08:09 dpw
//

#pragma once

namespace datetimelib {
    // unix timestamp
    unsigned int timestamp_seconds();

    // the time in milliseconds
    unsigned long timestamp_millis();

    // returns the local datetime in iso8601 format
    Str get_local_datetime();
}
