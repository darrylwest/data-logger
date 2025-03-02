//
// 2024-12-24 12:51:24 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <app/cfgsvc.hpp>
#include <app/client.hpp>
#include <app/database.hpp>
#include <app/exceptions.hpp>
#include <app/jsonkeys.hpp>
#include <app/taskrunner.hpp>
#include <app/temperature.hpp>
#include <app/version.hpp>
#include <app/webhandlers.hpp>
#include <nlohmann/json.hpp>
#include <random>
#include <ranges>
#include <vendor/testlib.hpp>

using namespace rcstestlib;

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

    unsigned int ts_in_the_past = 1738353093;
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
        r.fail("task should not get this far");
    } catch (std::exception& e) {
        spdlog::info("ex: {}", e.what());
        r.pass();
    }

    spdlog::set_level(spdlog::level::off);

    return r;
}

const app::client::ClientNode create_test_client() {
    const Str json_text
        = R"({"status":{"version":"test","ts":1738453678,"started":1738012925,"uptime":"0 days, 00:00:00","access":0,"errors":0}})";

    app::client::ClientStatus status = app::client::parse_status(json_text);
    const auto node = app::client::ClientNode{
        .location = "test",
        .ip = "10.0.1.115",
        .port = 2030,
        .active = true,
        .last_access = 0,
        .probes = {},
        .status = status,
    };

    return node;
}

// this really needs a mock
void test_fetch_client_status(Results& r) {
    // spdlog::set_level(spdlog::level::info);
    auto node = create_test_client();

    app::client::ClientStatus status = app::client::fetch_status(node);
    spdlog::info("status: ", status.to_string());

    r.equals(status.version.starts_with("0.6"), "the actual esp32 client version");
}

void test_fetch_temps(Results& r) {
    // spdlog::set_level(spdlog::level::info);

    auto node = create_test_client();
    auto data = app::client::fetch_temps(node);
    spdlog::info("temps: {}", data.to_string());

    r.equals(data.probes.size() > 0, "probe count");
}

void test_put_temps(Results& r) {
    // spdlog::set_level(spdlog::level::info);

    // TODO : use mock node and data for this
    auto url = "http://badhost:9090";
    auto node = create_test_client();
    auto data = app::client::fetch_temps(node);
    time_t timestamp = 1739563051;
    auto key = app::database::create_key(timestamp, "tmp.0");
    spdlog::info("temps: {}", data.to_string());

    r.equals(data.probes.size() > 0, "probe count");
    auto probe = data.probes.at(0);
    bool ok = app::client::put_temps(url, key, probe);
    r.equals(!ok, "should fail to put data");
}

Results test_client() {
    Results r = {.name = "Client Tests"};

    // spdlog::set_level(spdlog::level::info);

    // TODO : create mock node and data for this
    // skip these two if client node is dead...
    if (true) {
        test_fetch_temps(r);
        test_fetch_client_status(r);
        test_put_temps(r);
    } else {
        r.skip(true);
        r.skip(true);
    }

    spdlog::set_level(spdlog::level::off);

    return r;
}

void test_create_key(Results& r) {
    using namespace app::database;

    time_t timestamp = 1739563051;
    const auto location = "shed.0";
    DbKey key = create_key(timestamp, location);

    r.equals(key.timestamp == timestamp, "create key");
    // r.equals(key.type == ReadingType::Value::Temperature, "reading type");
    // r.equals(ReadingType::to_label(key.type) == "Temperature", "reading type label");
    // r.equals(ReadingType::to_value(key.type) == 1, "reading type int value");

    r.equals(key.location == location, "probe sensor key");

    spdlog::info("key: {}", key.to_string());
    r.equals(key.to_string() == "1739563051.shed.0", "date/probe sensor key");
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

    spdlog::debug("values: {} {} {} {}", t1, t2, t3, t4);

    for (int i = 0; i < size; ++i) {
        Str key, value;

        key = fmt::format("202502{:02d}{:02d}{:02d}{}", day, hour, minute, ".tmp.0");

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
    size_t size = (60 * 24);
    populate_database(db, size);

    r.equals(db.size() == size, "db size");

    auto keys = db.keys();
    r.equals(keys.size() == size, "db keys size");

    auto ok = db.save("/tmp/test.db");
    r.equals(ok, "save the database");
}

void test_append_key_value(Results& r) {
    // spdlog::set_level(spdlog::level::info);

    const auto filename = "/tmp/append.db";
    const auto key = app::database::create_key(1739563051, "tmp.0");
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
    spdlog::set_level(spdlog::level::err);

    using namespace app::database;
    const auto filename = "data/temperature/current.cottage.test";
    Database db;

    // read the current file
    db.read(filename, true);

    spdlog::info("dbsize: {}", db.size());
    const auto keys = db.keys();
    r.equals(keys.size() == db.size(), "size matters");

    // time the sort...
    if (false) {
        auto kylist = keys;
        const auto t0 = std::chrono::high_resolution_clock::now();
        std::ranges::sort(kylist);
        const auto t1 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        spdlog::info("sort of {} keys took {} µs", keys.size(), duration);
    }

    if (false) {                          // select to get a range of keys (slow)
        const auto start = "1740307200";  // .cottage.0";
        Vec<Str> kylist;
        const auto t0 = std::chrono::high_resolution_clock::now();
        for (const auto& key : keys) {
            if (key > start) {
                spdlog::debug("{}", key);
                kylist.push_back(key);
            }
        }
        const auto t1 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        spdlog::info("selection of {} keys from {} took {} µs", kylist.size(), keys.size(), duration);

        spdlog::info("kylist size: {}", kylist.size());
        r.equals(kylist.size() > 20, "kylist should be at least 20 element long");
    }

    {  // range drop
        const auto t0 = std::chrono::high_resolution_clock::now();

        // Ensure we don't drop more elements than exist
        const auto kylist = keys | std::views::drop(keys.size() > 25 ? keys.size() - 25 : 0);

        const auto t1 = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        spdlog::info("range drop of {} keys from {} took {} µs", kylist.size(), keys.size(), dur);

        for (const auto& key : kylist) {
            const auto value = db.get(key);
            spdlog::debug("key: {} {}", key, value);
        }
        r.equals(kylist.size() == 25, "should be exactly 25 keys");
    }

    {
        // map (transform) temperature strings to Vec<float> and keys to Vec<isodate>
        const auto all_keys = [](const Str&) { return true; };

        const auto t0 = std::chrono::high_resolution_clock::now();
        const auto data = db.search(all_keys);
        const auto t1 = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        spdlog::info("return data {} k/v's from took {} µs", data.size(), dur);

        r.equals(data.size() == db.size(), "data map element count should equal db.size");
    }

    {
        // map (transform) temperature strings to Vec<float> and keys to Vec<isodate>
        // TODO get the last element, track back 25 * 5 minutes; use that as a start
        const auto kylist = keys | std::views::drop(keys.size() > 25 ? keys.size() - 25 : 0);
        const auto start = *kylist.begin();
        const auto key_filter = [&](const Str& key) { return key >= start; };

        const auto t0 = std::chrono::high_resolution_clock::now();
        const auto data = db.search(key_filter);
        const auto t1 = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        spdlog::info("return data {} k/v's from took {} µs", data.size(), dur);

        for (const auto& [k, v] : data) {
            spdlog::debug("{} {}", k, v);
        }

        r.equals(data.size() == 25, "data map element count should equal ");
    }

    {
        const auto last = db.last();
        r.equals(last.size() == 1, "the default for last size should be a single element");

        const auto t0 = std::chrono::high_resolution_clock::now();
        const auto last25 = db.last(25);
        const auto t1 = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        spdlog::info("return last 25 of {} k/v's from took {} µs", db.size(), dur);

        r.equals(last25.size() == 25, "last 25 size should be 25 elements");
    }
    spdlog::set_level(spdlog::level::off);
}

Results test_database() {
    Results r = {.name = "Database Tests"};

    // spdlog::set_level(spdlog::level::info);

    test_create_key(r);
    test_database_data(r);
    test_append_key_value(r);
    test_read_current(r);

    spdlog::set_level(spdlog::level::off);

    return r;
}

void test_api_temps(Results& r) {
    spdlog::set_level(spdlog::level::err);

    using namespace app::database;
    const auto filename = "data/temperature/current.cottage.test";
    Database db;

    // read the current file
    db.read(filename, true);
    const auto temps = db.last(25);
    r.equals(temps.size() == 25, "get the last 25 readings");

    for (const auto& [k, v] : temps) {
        spdlog::info("{} {}", k, v);
    }

    spdlog::set_level(spdlog::level::off);
}

void test_create_chart_data(Results& r) {
    spdlog::set_level(spdlog::level::off);
    using namespace app::database;
    using namespace app::webhandlers;

    const auto filename = "data/temperature/current.cottage.test";
    Database db;
    db.read(filename, true);

    const std::time_t ts = 1740427800;
    const Vec<Str> locations = {"cottage.0"};
    const auto cfg = ChartConfig{
        .end_ts = ts,
        .locations = locations,
        .data_points = 25,
    };

    const auto chart = create_chart_data(db, cfg);
    spdlog::info("end date: {}", chart.end_date);

    r.equals(chart.end_date == "24-Feb-2025", "the end date should be feb");
    r.equals(chart.start_date == "24-Feb-2025", "the start date should be feb");

    r.equals(chart.labels.size() == 25, "number of labels should be 25");
    r.equals(chart.temps.size() == 1, "should be 1 locations for temp data F");
    for (const auto& [k, v] : chart.temps) {
        spdlog::info("loc: {} vec: {}", k, v.size());
    }

    spdlog::set_level(spdlog::level::off);
}

Results test_webhandlers() {
    Results r = {.name = "Service Tests"};

    test_api_temps(r);
    test_create_chart_data(r);

    spdlog::set_level(spdlog::level::off);
    return r;
}

Results test_service() {
    // using json = nlohmann::json;

    Results r = {.name = "Service Tests"};

    spdlog::set_level(spdlog::level::off);

    // create tests
    r.pass();

    spdlog::set_level(spdlog::level::off);
    return r;
}

int main() {
    using namespace colors;
    // spdlog::set_level(spdlog::level::error); // or off
    spdlog::set_level(spdlog::level::off);

    const auto vers = app::Version().to_string();
    fmt::print("\nExample Unit Tests, Version: {}{}{}\n\n", cyan, vers, reset);

    Results summary = Results{.name = "Unit Test Summary"};

    // lambda to run a test and add its result to the summary
    auto run_test = [&summary](auto test_func) {
        auto result = test_func();
        fmt::print("{}\n", result.to_string());
        summary.add(result);
    };

    run_test(test_taskrunner);
    run_test(test_client);
    run_test(test_database);
    run_test(test_webhandlers);
    run_test(test_service);

    fmt::print("\n{}", summary.to_string());
    auto msg = (summary.failed == 0) ? green + "Ok" : "\n" + red + "Tests failed!";
    fmt::print("\nUnit Test Results: {}{}{}\n", cyan, msg, reset);

    return 0;
}
