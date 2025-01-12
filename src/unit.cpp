//
// 2024-12-24 12:51:24 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

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
