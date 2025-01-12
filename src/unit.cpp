//
// 2024-12-24 12:51:24 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/temperature.hpp>
#include <app/version.hpp>
#include <vendor/testlib.hpp>

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

// put this in unit tests
const auto createSampleReading() {
    std::string text =
          "readings:2025-01-12T13:24:23\n"
          "version:2024.02.08\n"
          "ts:1736717063\n"
          "temp:1736717063:tempC:13.44, tempF:56.19\n"
          "light:1736717063:0\n"
          "status:started:1736712734, uptime:0 days 1:12:09, access:0, errors:1";

    return text;
}

Results test_temperature() {
    Results r = {.name = "Version Tests"};
    const auto reading = createSampleReading();
    app::TemperatureData data = app::parse_reading(reading);

    r.equals(data.date == "2025-01-12T13:24:23");

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

    fmt::println("\n{}", summary.to_string());
    auto msg = (summary.failed == 0) ? green + "Ok" : "\n" + red + "Tests failed!";
    fmt::println("\nUnit Test Results: {}{}{}", cyan, msg, reset);

    return 0;
}
