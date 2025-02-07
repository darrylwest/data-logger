//
// 2025-02-07 00:08:09 dpw
//

#pragma once

namespace datetime {
    // unix timestamp
    unsigned int timestamp_seconds();

    // returns the local datetime in iso8601 format
    Str get_local_datetime();
}
