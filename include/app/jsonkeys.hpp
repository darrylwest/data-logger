//
// 2025-02-09 15:51:10 dpw
//

#pragma once

#include <app/types.hpp>

namespace app {
    namespace jsonkeys {
        constexpr StrView WEBSERVICE = "webservice";
        constexpr StrView HOST = "host";
        constexpr StrView WWW = "www";
        constexpr StrView TLS_CERT_FILE = "tls_cert_file";
        constexpr StrView TLS_KEY_FILE = "tls_key_file";
        constexpr StrView SCHEME = "scheme";
        constexpr StrView CLIENT_STATUS = "status";
        constexpr StrView VERSION = "version";
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
    }  // namespace jsonkeys
}  // namespace app
