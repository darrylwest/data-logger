//
// 2024-12-24 12:51:24 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/client.hpp>
#include <app/temperature.hpp>
#include <app/version.hpp>
#include <toml.hpp>
#include <vendor/taskrunner.hpp>
#include <vendor/testlib.hpp>

using namespace rcstestlib;

constexpr float EPSILON = 1e-5f;

Results test_version() {
    Results r = {.name = "Version Tests"};

    auto vers = app::Version();
    r.equals(vers.major == 0);
    r.equals(vers.minor == 2);
    r.equals(vers.patch == 2);
    r.equals(vers.build >= 100);

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
    auto task = taskrunner::createTask("test-task", period, worker);

    int ts_in_the_past = 1738353093;
    r.equals(task.name == "test-task", "name should match");
    r.equals(task.started_at > ts_in_the_past, "name should match");
    r.equals(task.run_count == 0, "never run");
    r.equals(task.period == period, "period should match");
    r.equals(counter == 0, "initial value");
    // invoke the task runner
    task.runner();
    r.equals(counter == 1, "new value changed by worker");

    auto tstr = task.to_string();
    r.equals(tstr != "", "should match");

    unsigned long tms = taskrunner::timestamp_millis();
    unsigned long tss = taskrunner::timestamp_seconds();
    spdlog::info("tms: {}, tss: {}", tms, tss);
    r.equals(tms / 10000 == tss / 10, "times should match");

    spdlog::set_level(spdlog::level::off);

    return r;
}

// put this in unit tests
const auto createSampleReading() {
    std::string text = R"({"reading_at":{"time":"2025-01-31T14:27:46","ts":1738362466},
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
    app::TemperatureData data = app::parse_reading(reading);

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

    std::string json_text
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
        std::string name = toml::find<std::string>(config, "name");
        r.equals(name == "temperature", "name should match");

        // TODO pull out the temperature locations
        auto locations = toml::find<std::vector<toml::value>>(config, "locations");
        r.equals(locations.size() >= 2, "should be at least two locations");

        for (const auto& loc : locations) {
            std::string location = toml::find<std::string>(loc, "location");
            std::string ip = toml::find<std::string>(loc, "ip");
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

            auto probes = toml::find<std::vector<toml::value>>(loc, "probes");
            r.equals(probes.size() >= 1, "probes for each location");

            for (const auto& probe : probes) {
                int sensor = toml::find<int>(probe, "sensor");
                std::string probe_location = toml::find<std::string>(probe, "location");

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

    fmt::println("\n{}", summary.to_string());
    auto msg = (summary.failed == 0) ? green + "Ok" : "\n" + red + "Tests failed!";
    fmt::println("\nUnit Test Results: {}{}{}", cyan, msg, reset);

    return 0;
}
