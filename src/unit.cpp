//
// 2024-12-24 12:51:24 dpw
//

#include <spdlog/spdlog.h>

#include <app/cfgsvc.hpp>
#include <app/database.hpp>
#include <app/exceptions.hpp>
#include <app/taskrunner.hpp>
#include <app/version.hpp>
#include <app/webhandlers.hpp>
#include <vendor/perftimer.hpp>
#include <vendor/testlib.hpp>

#include "precompiled.hpp"

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

// TODO move some of this to integration tests
void test_read_current(Results& r) {
    spdlog::set_level(spdlog::level::critical);
    perftimer::PerfTimer timer("test_read_current");

    using namespace app::database;
    const auto filename = "data/current.temps.test";
    Database db;

    // read the current file
    db.read(filename, true);

    spdlog::info("dbsize: {}", db.size());
    const auto keys = db.keys();
    r.equals(keys.size() == db.size(), "size matters");

    // time the sort...
    if (true) {
        auto kylist = keys;
        timer.start();
        std::ranges::sort(kylist);
        timer.stop();
        spdlog::info("sort of {} keys took {} µs", keys.size(), timer.get_duration().count());
    }

    if (true) {                           // select to get a range of keys (slow)
        const auto start = "1740307200";  // .cottage.0";
        Vec<Str> kylist;
        timer.start();
        for (const auto& key : keys) {
            if (key > start) {
                spdlog::debug("{}", key);
                kylist.push_back(key);
            }
        }
        timer.stop();
        auto duration = timer.get_duration().count();
        spdlog::info("selection of {} keys from {} took {} µs", kylist.size(), keys.size(),
                     duration);

        spdlog::info("kylist size: {}", kylist.size());
        r.equals(kylist.size() > 20, "kylist should be at least 20 element long");
    }

    {  // range drop
        timer.start();

        // Ensure we don't drop more elements than exist
        const auto kylist = keys | std::views::drop(keys.size() > 25 ? keys.size() - 25 : 0);

        timer.stop();
        auto dur = timer.get_duration().count();
        spdlog::info("range drop of {} keys from {} took {} µs", kylist.size(), keys.size(), dur);

        for (const auto& key : kylist) {
            const auto ret = db.get(key);
            if (ret) {
                const auto value = *ret;
                spdlog::debug("key: {} {}", key, value);
            } else {
                spdlog::warn("key: {} not found", key);
                r.fail("not found for key");
            }
        }

        r.equals(kylist.size() == 25, "should be exactly 25 keys");
    }

    {
        // map (transform) temperature strings to Vec<float> and keys to Vec<isodate>
        const auto all_keys = [](const Str&) { return true; };

        timer.start();
        const auto data = db.search(all_keys);
        timer.stop();
        auto dur = timer.get_duration().count();
        spdlog::info("return data {} k/v's from took {} µs", data.size(), dur);

        r.equals(data.size() == db.size(), "data map element count should equal db.size");
    }

    {
        // map (transform) temperature strings to Vec<float> and keys to Vec<isodate>
        // TODO get the last element, track back 25 * 5 minutes; use that as a start
        const auto kylist = keys | std::views::drop(keys.size() > 25 ? keys.size() - 25 : 0);
        const auto start = *kylist.begin();
        const auto key_filter = [&](const Str& key) { return key >= start; };

        timer.start();
        const auto data = db.search(key_filter);
        timer.stop();
        auto dur = timer.get_duration().count();
        spdlog::info("return data {} k/v's from took {} µs", data.size(), dur);

        for (const auto& [k, v] : data) {
            spdlog::debug("{} {}", k, v);
        }

        r.equals(data.size() == 25, "data map element count should equal ");
    }

    {
        const auto last = db.last();
        r.equals(last.size() == 1, "the default for last size should be a single element");

        timer.start();
        const auto last25 = db.last(25);
        timer.stop();
        auto dur = timer.get_duration().count();
        spdlog::info("return last 25 of {} k/v's from took {} µs", db.size(), dur);

        r.equals(last25.size() == 25, "last 25 size should be 25 elements");
    }

    spdlog::set_level(spdlog::level::critical);
}

void test_api_temps(Results& r) {
    spdlog::set_level(spdlog::level::err);

    using namespace app::database;
    const auto filename = "data/current.temps.test";
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

    const auto filename = "data/current.temps.test";
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
    Results r = {.name = "WebHandlers Tests"};

    test_api_temps(r);
    test_create_chart_data(r);

    spdlog::set_level(spdlog::level::off);
    return r;
}

Results test_database() {
    Results r = {.name = "Database Tests"};

    test_read_current(r);

    return r;
}

Results test_clients() {
    using namespace app;

    Results r = {.name = "Client Tests"};
    auto nodes = client::create_nodes();

    r.equals(nodes.size() == 3, "should be 3 client nodes from config");

    for (const auto& node : nodes) {
        auto client = client::parse_client_node(cfgsvc::client(node.location));
        if (!client.active) {
            continue;
        }

        try {
            // spdlog::set_level(spdlog::level::info);
            // const auto t0 = std::chrono::high_resolution_clock::now();
            auto status = client::fetch_status(client);
            // const auto t1 = std::chrono::high_resolution_clock::now();
            r.equals(status.version.starts_with("0.6."), "should be version 0.6.x");
            r.equals(status.access_count > 0, "should have access counts");
            r.equals(status.errors == 0, "should have zero errors");
            const auto ts = status.timestamp;

            r.equals(status.location.size() > 1, "location should be set");

            auto temps = client::fetch_temps(client);
            r.equals(temps.probes.size() >= 2, "should have probes");
            r.equals(temps.reading_at >= ts, "should have reading at timestamp");
        } catch (const std::exception& e) {
            r.fail(fmt::format("exception: ", e.what()));
        }
    }

    spdlog::set_level(spdlog::level::off);

    return r;
}

Results test_service() {
    // using json = nlohmann::json;

    Results r = {.name = "Service Tests"};

    spdlog::set_level(spdlog::level::off);

    // create tests
    r.pass();

    // test_fetch_status
    // test_fetch_temps
    // test_put_temps

    spdlog::set_level(spdlog::level::off);
    return r;
}

int main() {
    using namespace colors;

    spdlog::set_level(spdlog::level::critical);

    app::cfgsvc::ServiceContext ctx;
    ctx.sleep_duration = std::chrono::seconds(0);  // don't start the loop

    app::cfgsvc::configure(ctx);

    const auto vers = app::Version().to_string();
    fmt::print("\nUnit/Integration Tests, Version: {}{}{}\n\n", cyan, vers, reset);

    Results summary = Results{.name = "Unit/Integration Test Summary"};

    // lambda to run a test and add its result to the summary
    auto run_test = [&summary](auto test_func) {
        auto result = test_func();
        fmt::print("{}\n", result.to_string());
        summary.add(result);
    };

    // run_test(test_taskrunner);
    run_test(test_webhandlers);
    run_test(test_database);
    run_test(test_service);
    run_test(test_clients);

    fmt::print("\n{}", summary.to_string());
    auto msg = (summary.failed == 0) ? bright::green + "Ok" : "\n" + bright::red + "Tests failed!";
    fmt::print("\nUnit/Integration Test Results: {}{}{}\n\n", cyan, msg, reset);

    return 0;
}
