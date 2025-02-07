//
// 2024-12-24 12:51:24 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/client.hpp>
#include <app/database.hpp>
#include <app/datetimelib.hpp>
#include <app/exceptions.hpp>
#include <app/service.hpp>
#include <app/taskrunner.hpp>
#include <app/temperature.hpp>
#include <app/version.hpp>
#include <nlohmann/json.hpp>
#include <random>
#include <toml.hpp>
#include <vendor/testlib.hpp>

using namespace rcstestlib;

Results test_version() {
    Results r = {.name = "Version Tests"};

    auto vers = app::Version();
    r.equals(vers.major == 0);
    r.equals(vers.minor == 2);
    r.equals(vers.patch == 5);
    r.equals(vers.build >= 140);

    return r;
}

Results test_taskrunner() {
    Results r = {.name = "TaskRunner Tests"};

    // spdlog::set_level(spdlog::level::info);

    int counter = 0;
    auto worker = [&counter]() {
        counter++;
        spdlog::info("my task count: {}", counter);
    };

    int period = 15;  // in seconds
    auto task = app::taskrunner::createTask("test-task", period, worker);

    int ts_in_the_past = 1738353093;
    r.equals(Str(task.name) == "test-task", "name should match");
    r.equals(task.started_at > ts_in_the_past, "name should match");
    r.equals(task.run_count == 0, "never run");
    r.equals(task.period == period, "period should match");
    r.equals(counter == 0, "initial value");
    // invoke the task runner
    task.runner();
    r.equals(counter == 1, "new value changed by worker");

    auto tstr = task.to_string();
    r.equals(tstr != "", "should match");

    // spdlog::set_level(spdlog::level::info);
    // test the task exception
    auto ex_worker = []() {
        spdlog::info("ex worker will throw a service exception");
        throw app::ServiceException("Service Exception, Worker failed");
    };

    auto ex_task = app::taskrunner::createTask("exception-task", 2, ex_worker);

    try {
        app::taskrunner::run(ex_task.runner, ex_task.name, ex_task.period);
        r.fail("should not get this far");
    } catch (std::exception& e) {
        spdlog::info("ex: {}", e.what());
        r.pass("should catch here");
    }

    spdlog::set_level(spdlog::level::off);

    return r;
}

// put this in unit tests
const auto createSampleReading() {
    Str text = R"({"reading_at":{"time":"2025-01-31T14:27:46","ts":1738362466},
            "probes":[
                {"sensor":0,"location":"cottage-south","millis":349548023,"tempC":10.88542,"tempF":51.59375},
                {"sensor":1,"location":"cottage-east","millis":349548023,"tempC":10.92542,"tempF":51.66576}
            ]}
        )";

    return text;
}

Results test_temperature() {
    Results r = {.name = "Temperature Reading Tests"};

    // spdlog::set_level(spdlog::level::info);

    const auto reading = createSampleReading();
    app::TemperatureData data = app::temperature::parse_reading(reading);

    r.equals(data.reading_at == "2025-01-31T14:27:46", "reading date should be set");
    r.equals(data.timestamp == 1738362466, "timestamp should match");
    r.equals(data.probes.size() == 2, "should be two probes");
    // now test the probe data; sensor, location, tempC, tempF
    auto probe0 = data.probes.at(0);
    r.equals(probe0.sensor == 0, "sensor id 0");
    r.equals(probe0.location == "cottage-south", "probe0 location");
    r.equals(std::abs(probe0.tempC - 10.88542) < EPSILON, "probe0 tempC");
    r.equals(std::abs(probe0.tempF - 51.59375) < EPSILON, "probe0 tempC");

    auto probe1 = data.probes.at(1);
    r.equals(probe1.sensor == 1, "sensor id 1");
    r.equals(probe1.location == "cottage-east", "probe1 location");
    r.equals(std::abs(probe1.tempC - 10.92542) < EPSILON, "probe0 tempC");
    r.equals(std::abs(probe1.tempF - 51.66576) < EPSILON, "probe0 tempC");

    spdlog::set_level(spdlog::level::off);

    return r;
}

app::client::ClientNode create_test_client() {
    auto node = app::client::ClientNode{
        .location = "test",
        .ip = "10.0.1.197",
        .port = 2030,
        .active = true,
        .last_access = 0,
    };

    return node;
}

void test_parse_client_status(Results& r) {
    // spdlog::set_level(spdlog::level::info);

    Str json_text
        = R"({"status":{"version":"0.5.26-135","ts":1738453678,"started":1738012925,"uptime":"5 days, 02:25:53","access":8247,"errors":0}})";

    app::client::ClientStatus status = app::client::parse_status(json_text);
    spdlog::info("status: {}", status.to_string());

    r.equals(status.version == "0.5.26-135", "the version");
    r.equals(status.timestamp == 1738453678, "the timestamp");
    r.equals(status.started == 1738012925, "started");
    r.equals(status.uptime == "5 days, 02:25:53", "uptime");
    r.equals(status.access_count == 8247, "access");
    r.equals(status.errors == 0, "errors");

    spdlog::set_level(spdlog::level::off);
}

// this really needs a mock
void test_fetch_client_status(Results& r) {
    auto node = create_test_client();

    app::client::ClientStatus status = app::client::fetch_status(node);
    spdlog::info("status: ", status.to_string());

    r.equals(status.version == "0.5.26-135", "the version");
}

void test_fetch_temps(Results& r) {
    // spdlog::set_level(spdlog::level::info);

    auto node = create_test_client();
    auto data = app::client::fetch_temps(node);
    spdlog::info("temps: {}", data.to_string());

    r.equals(data.probes.size() > 0, "probe count");
}

Results test_client() {
    Results r = {.name = "Client Tests"};

    test_parse_client_status(r);

    // skip these two if client node is dead...
    if (true) {
        test_fetch_client_status(r);
        test_fetch_temps(r);
    } else {
        r.skip(true);
        r.skip(true);
    }

    spdlog::set_level(spdlog::level::off);

    return r;
}

Results test_config() {
    Results r = {.name = "Config Tests"};

    // spdlog::set_level(spdlog::level::info); // or off

    try {
        // parse the config file
        auto config = toml::parse("./config/test-config.toml");

        // Verify the parsed data
        Str name = toml::find<Str>(config, "name");
        r.equals(name == "temperature", "name should match");

        // TODO pull out the temperature locations
        auto locations = toml::find<Vec<toml::value>>(config, "locations");
        r.equals(locations.size() >= 2, "should be at least two locations");

        for (const auto& loc : locations) {
            Str location = toml::find<Str>(loc, "location");
            Str ip = toml::find<Str>(loc, "ip");
            int port = toml::find<int>(loc, "port");
            bool active = toml::find<bool>(loc, "active");

            spdlog::info("loc: {} ip: {} port: {}, active: {}", location, ip, port, active);

            r.equals(location.size() > 2, "location text");

            if (location == "cottage") {
                r.equals(ip == "10.0.1.197", "cottage location ip");
                r.equals(port == 2030, "port");
            } else if (location == "shed") {
                r.equals(ip == "10.0.1.115", "shed location ip");
                r.equals(port == 2030, "port");
            } else {
                r.equals(false, "not a valid location");
            }

            auto probes = toml::find<Vec<toml::value>>(loc, "probes");
            r.equals(probes.size() >= 1, "probes for each location");

            for (const auto& probe : probes) {
                int sensor = toml::find<int>(probe, "sensor");
                Str probe_location = toml::find<Str>(probe, "location");

                spdlog::info("sensor: {} location: {}", sensor, probe_location);

                r.equals(sensor >= 0, "sensor 0 or 1");
                r.equals(probe_location.size() > 3, "probe location");
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "TOML ERROR: an error occurred: " << e.what() << std::endl;
        r.equals(false, "fail exception");
    }

    spdlog::set_level(spdlog::level::off);  // or off

    return r;
}

bool bad_db_file(const Str& filename) {
    if (filename == "bad_db_file") {
        throw app::DatabaseException("Db Failed to connect to file: " + filename);
    } else if (filename == "bad_filename") {
        throw app::FileException("Failed to open: " + filename);
    }

    // return false; should never get here
    return false;
}

bool bad_http_get(const Str& url) {
    if (url == "http://bad_network.com") {
        throw app::NetworkException("Failed to connect to url: " + url);
    } else if (url == "http://service_down.com") {
        throw app::ServiceException("Service Failed for url: " + url);
    } else if (url == "http://bad_parse.com") {
        throw app::ParseException("JSON parse failed at url: " + url);
    }

    // return false; should never get here
    return false;
}

Results test_exceptions() {
    Results r = {.name = "Exception Tests"};

    // spdlog::set_level(spdlog::level::info);

    try {
        bool ok = bad_http_get("http://bad_network.com");
        r.fail("should not get here from a bad http get");
    } catch (const std::exception& e) {
        spdlog::info("exception: {}", e.what());
        r.pass("exception throw ok");
    }

    try {
        bool ok = bad_http_get("http://service_down.com");
        r.equals(ok, "should not get here");
    } catch (const std::exception& e) {
        spdlog::info("exception: {}", e.what());
        r.equals(true);
    }

    try {
        bool ok = bad_http_get("http://bad_parse.com");
        r.equals(ok, "should not get here");
    } catch (const std::exception& e) {
        spdlog::info("exception: {}", e.what());
        r.equals(true);
    }

    try {
        bool ok = bad_db_file("bad_db_file");
        r.equals(ok, "should not get here");
    } catch (const std::exception& e) {
        spdlog::info("exception: {}", e.what());
        r.equals(true);
    }

    try {
        bool ok = bad_db_file("bad_filename");
        r.equals(ok, "should not get here");
    } catch (const std::exception& e) {
        spdlog::info("exception: {}", e.what());
        r.equals(true);
    }

    spdlog::set_level(spdlog::level::off);

    return r;
}

void test_parse_datetime(Results& r) {
    // spdlog::set_level(spdlog::level::info);
    using namespace app::database;

    Str datetime = "2025-02-04T11:40:23";

    auto dt = parse_datetime(datetime);
    r.equals(dt == "202502041140", "parse date time for key");
    r.equals(dt.size() == 12, "dt size");

    const time_t ts = 1738779849;
    const auto isodate = timestamp_to_local(ts);
    spdlog::info("ts: {}, dt: {}", ts, isodate);

    spdlog::set_level(spdlog::level::off);
}

void test_create_key(Results& r) {
    using namespace app::database;

    Str datetime = "2025-02-04T11:40:23";
    DbKey key = create_key(datetime, "cottage-south");

    r.equals(key.datetime == "202502041140", "create key");
    // r.equals(key.type == ReadingType::Value::Temperature, "reading type");
    // r.equals(ReadingType::to_label(key.type) == "Temperature", "reading type label");
    // r.equals(ReadingType::to_value(key.type) == 1, "reading type int value");

    r.equals(key.location == "cottage-south", "probe location");

    spdlog::info("key: {}", key.to_string());
    r.equals(key.to_string() == "202502041140.cottage-south", "date/probe key");
}

// Unit test method to populate with random data
void populate_database(app::database::Database& db, int size = 500) {
    std::random_device rd;
    std::mt19937 gen(rd());

    int day = 1;
    int hour = 0;
    int minute = 0;
    float t1 = 9.00;
    float t2 = 8.94;
    float t3 = 9.04;
    float t4 = 9.06;

    for (int i = 0; i < size; ++i) {
        Str key, value;

        key = fmt::format("202502{:02d}{:02d}{:02d}{}", day, hour, minute, ".cottage-south");

        minute++;
        if (minute > 59) {
            hour++;
            minute = 0;
        }

        if (hour > 23) {
            day++;
            hour = 0;
        }

        // value = fmt::format("{:6f} {:6f} {:6f} {:6f}", t1, t2, t3, t4);
        value = fmt::format("{:6f}", t1);

        if (i < size / 2) {
            t1 += 0.01;
            t2 += 0.005;
            t3 += 0.007;
            t4 += 0.01;
        } else {
            t1 -= 0.01;
            t2 -= 0.005;
            t3 -= 0.007;
            t4 -= 0.01;
        }

        if (!db.set(key, value)) {
            throw app::DatabaseException("error putting key/value");
        }
    }

    spdlog::info("Database populated with {} random key-value pairs.", size);
}

void test_database_data(Results& r) {
    app::database::Database db = app::database::Database();
    r.equals(db.size() == 0, "db empty size");

    // 1 days of data
    int size = (60 * 24);
    populate_database(db, size);

    r.equals(db.size() == size, "db size");

    auto keys = db.keys();
    r.equals(keys.size() == size, "db keys size");

    auto ok = db.save("/tmp/test.db");
    r.equals(ok, "save the database");
}

void test_truncate_to_minute(Results& r) {
    using namespace app::database;

    Str isodate = "2025-02-05T07:49:22";
    Str truncated = truncate_to_minutes(isodate);
    r.equals(truncated == "2025-02-05T07:45", "45 minute");

    isodate = "2025-02-05T07:51:22";
    truncated = truncate_to_minutes(isodate);
    r.equals(truncated == "2025-02-05T07:50", "50 minute");

    isodate = "2025-02-05T08:01:22";
    truncated = truncate_to_minutes(isodate);
    r.equals(truncated == "2025-02-05T08:00", "top of hour minute");
}

void test_append_key_value(Results& r) {
    // spdlog::set_level(spdlog::level::info);

    const auto filename = "/tmp/append.db";
    const auto key = app::database::create_key("2025-02-36T07:36", "test-location");
    const auto value = "7.553";
    app::database::append_key_value(filename, key, value);

    r.equals(true, "didn't throw");
    // TODO read the file back and verify key/value

    try {
        const auto badfile = "bad-file/folder/temps/bad.db";
        app::database::append_key_value(badfile, key, value);
        r.equals(false, "fail: should throw on filename: bad-file/...");
    } catch (const app::FileException& e) {
        r.equals(true, "should throw");
        spdlog::error("bad file: {}", e.what());
    }
}

void test_read_current(Results& r) {
    // spdlog::set_level(spdlog::level::info);

    using namespace app::database;
    const auto filename = "data/temperature/current.deck-west.test";
    Database db;

    // read the current file
    db.read(filename, true);

    spdlog::info("dbsize: {}", db.size());
    const auto keys = db.keys();
    r.equals(keys.size() == db.size(), "size matters");

    // get just selected keys
    const auto noon_keys = db.keys("2025020512");
    r.equals(noon_keys.size() == 12, "always 12 entries for the hour");
    for (const auto& key : db.keys("2025020512")) {
        spdlog::info("key: {}={}", key, db.get(key));
    }
}

Results test_database() {
    Results r = {.name = "Database Tests"};

    // spdlog::set_level(spdlog::level::info);

    test_parse_datetime(r);
    test_create_key(r);
    test_database_data(r);
    test_truncate_to_minute(r);
    test_append_key_value(r);
    test_read_current(r);

    spdlog::set_level(spdlog::level::off);

    return r;
}

Results test_service() {
    using json = nlohmann::json;

    Results r = {.name = "Service Tests"};

    spdlog::set_level(spdlog::level::off);

    Str end_date = "2025-02-06";
    Vec<Str> labels
        = {"09:30", "10:00", "10:30", "11:00", "11:30", "12:00", "12:30", "13:00", "13:30",
           "14:00", "14:30", "15:00", "15:30", "16:00", "16:30", "17:00", "17:30", "18:00",
           "18:30", "19:00", "19:30", "20:00", "20:30", "21:00", "21:30"};

    const auto json_text = app::create_temps_response(labels, end_date);
    const auto response = json::parse(json_text);

    spdlog::info("json: {}", json_text);

    Str ed = response["end_date"];
    Vec<Str> lbls = response["labels"];
    Vec<json> datasets = response["datasets"];

    r.equals(ed == end_date, "validate end date");
    r.equals(lbls == labels, "validate labels");
    r.equals(datasets.size() >= 2, "dataset size");

    spdlog::set_level(spdlog::level::off);
    return r;
}

Results test_datetimelib() {
    Results r = {.name = "Datetime Tests"};

    spdlog::set_level(spdlog::level::info);

    auto unix_ts = datetimelib::timestamp_seconds();
    spdlog::info("unix ts: {}", unix_ts);
    unsigned int tsz = 1738888855;
    r.equals(unix_ts > tsz, "unix ts should be in the future");

    unsigned long tszl = 1738888855000;
    auto mts = datetimelib::timestamp_millis();
    spdlog::info("millis ts: {}", mts);
    r.equals(mts > tszl, "millis ts should be in the future");

    unsigned long tms = datetimelib::timestamp_millis();
    unsigned long tss = datetimelib::timestamp_seconds();
    spdlog::info("tms: {}, tss: {}", tms, tss);
    r.equals(tms / 10000 == tss / 10, "times should match");

    auto iso_now = datetimelib::local_iso_datetime();
    spdlog::info("now local iso: {}", iso_now);

    auto iso_dt = datetimelib::local_iso_datetime(tsz);
    spdlog::info("local iso: {}", iso_dt);
    r.equals(iso_dt == "2025-02-06T16:40:55-0800", "iso time");

    return r;
}

int main(int argc, const char* argv[]) {
    using namespace colors;
    // spdlog::set_level(spdlog::level::error); // or off
    spdlog::set_level(spdlog::level::off);

    const auto vers = app::Version().to_string();
    fmt::println("\nExample Unit Tests, Version: {}{}{}\n", cyan, vers, reset);

    Results summary = Results{.name = "Unit Test Summary"};

    // lambda to run a test and add its result to the summary
    auto run_test = [&summary](auto test_func) {
        auto result = test_func();
        fmt::println("{}", result.to_string());
        summary.add(result);
    };

    run_test(test_version);
    run_test(test_taskrunner);
    run_test(test_temperature);
    run_test(test_client);
    run_test(test_config);
    run_test(test_exceptions);
    run_test(test_database);
    run_test(test_service);
    run_test(test_datetimelib);

    fmt::println("\n{}", summary.to_string());
    auto msg = (summary.failed == 0) ? green + "Ok" : "\n" + red + "Tests failed!";
    fmt::println("\nUnit Test Results: {}{}{}", cyan, msg, reset);

    return 0;
}
