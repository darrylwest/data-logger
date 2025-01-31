//
// 2024-12-24 12:51:24 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/temperature.hpp>
#include <app/version.hpp>
#include <vendor/testlib.hpp>
#include <vendor/taskrunner.hpp>

using namespace rcstestlib;

Results test_version() {
    Results r = {.name = "Version Tests"};

    auto vers = app::Version();
    r.equals(vers.major == 0);
    r.equals(vers.minor == 1);
    r.equals(vers.patch == 0);
    r.equals(vers.build >= 100);

    return r;
}

Results test_taskrunner() {
    Results r = {.name = "TaskRunner Tests"};

    // spdlog::set_level(spdlog::level::info);

    int period = 15; // in seconds
    auto task = taskrunner::createTask("test-task", period, []() {
        spdlog::info("in my task");
    });

    int ts_in_the_past = 1738353093;
    r.equals(task.name == "test-task", "name should match");
    r.equals(task.started_at > ts_in_the_past, "name should match");
    r.equals(task.run_count == 0, "never run");
    r.equals(task.period == period, "period should match");

    auto tstr = task.to_string();
    r.equals(tstr != "", "should match");

    unsigned long tms = taskrunner::timestamp_millis();
    unsigned long tss =  taskrunner::timestamp_seconds();
    spdlog::info("tms: {}, tss: {}", tms, tss);
    r.equals(tms / 10000 == tss / 10, "times should match");

    // spdlog::set_level(spdlog::level::off);
    return r;
}

// put this in unit tests
const auto createSampleReading() {
    std::string text
        = "readings:2025-01-12T13:24:23\n"
          "version:2024.02.08\n"
          "ts:1736717063\n"
          "temp:1736717063:tempC:13.44, tempF:56.19\n"
          "light:1736717063:0\n"
          "status:started:1736712734, uptime:0 days 1:12:09, access:0, errors:1";

    return text;
}

Results test_temperature() {
    Results r = {.name = "Version Tests"};
    const auto location = "test-location";
    const auto reading = createSampleReading();
    app::TemperatureData data = app::parse_reading(location, reading);

    r.equals(data.location == location, "location should be set");
    r.equals(data.reading_date == "2025-01-12T13:24:23", "date should be set");
    r.equals(data.tempC == 13.44, "temp c should be 13.44");
    r.equals(data.tempF == 56.19, "temp f should be 56.19");
    r.equals(data.isValid == true, "reading should be valid");

    return r;
}

int main(int argc, const char *argv[]) {
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

    fmt::println("\n{}", summary.to_string());
    auto msg = (summary.failed == 0) ? green + "Ok" : "\n" + red + "Tests failed!";
    fmt::println("\nUnit Test Results: {}{}{}", cyan, msg, reset);

    return 0;
}
