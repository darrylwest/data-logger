//
// 2025-02-09 15:51:10 dpw
//

#pragma once

#include <app/types.hpp>

namespace app::jsonkeys {
    constexpr StrView WEBSERVICE = "webservice";
    constexpr StrView HOST = "host";
    constexpr StrView WWW = "www";
    constexpr StrView TLS_CERT_FILE = "tls_cert_file";
    constexpr StrView TLS_KEY_FILE = "tls_key_file";
    constexpr StrView SCHEME = "scheme";
    constexpr StrView CLIENT_STATUS = "status";
    constexpr StrView VERSION = "version";
    constexpr StrView CONFIG_VERSION = "config-version";
    constexpr StrView TS = "ts";
    constexpr StrView STARTED = "started";
    constexpr StrView UPTIME = "uptime";
    constexpr StrView ACCESS = "access";
    constexpr StrView ERRORS = "errors";
    constexpr StrView CLIENTS = "clients";
    constexpr StrView LOCATION = "location";
    constexpr StrView IP = "ip";
    constexpr StrView PORT = "port";
    constexpr StrView ACTIVE = "active";
    constexpr StrView SENSORS = "sensors";
    constexpr StrView TYPE = "type";
    constexpr StrView TEMPERATURE = "temperature";
    constexpr StrView PROBES = "probes";
    constexpr StrView SENSOR = "sensor";
    constexpr StrView END_DATE = "end_date";
    constexpr StrView LABELS = "labels";
    constexpr StrView DATASETS = "datasets";
    constexpr StrView READING_AT = "reading_at";
    constexpr StrView ENABLED = "enabled";
    constexpr StrView TEMP_C = "tempC";
    constexpr StrView TEMP_F = "tempF";
}  // namespace app::jsonkeys
