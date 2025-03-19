//
// 2024-12-23 15:25:10 dpw
//

#include <httplib.h>
#include <spdlog/fmt/fmt.h>
#include <unistd.h>

#include <app/database.hpp>
#include <app/types.hpp>
#include <app/version.hpp>
#include <atomic>
#include <cstdio>
#include <datetimelib/datetimelib.hpp>
#include <nlohmann/json.hpp>
#include <thread>
#include <vendor/ansi_colors.hpp>
#include <vendor/testlib.hpp>

#include "precompiled.hpp"

using namespace colors;
using namespace rcstestlib;

struct Config {
    Str scheme = "http://";
    Str host = "localhost";
    Str port = "29099";
    bool start_server = true;
    Str logfile = "logs/integration-test.log";

    Str get_url() { return scheme + host + ":" + port; }
};

// Define the function to start the service
void run_server(std::atomic<bool>& running, const Config& config) {
    running = true;

    Str cmd = fmt::format("./build/data-logger --host {} --port {} > {} 2>&1 & echo $!",
                          config.host, config.port, config.logfile);

    fmt::print("{}Server Start Command: {}{}{}\n", cyan, yellow, cmd, reset);

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        fmt::print("{}File to start service.{}\n", red, reset);
        running = false;
        return;
    }

    // Read the process ID of the started service
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        pid_t pid = std::stoi(buffer);
        fmt::print("{}Service started with PID: {}{}\n", cyan, yellow, pid, reset);
    } else {
        fmt::print("\t{}Failed to retrieve service PID!\n", red, reset);
        running = false;
    }

    pclose(pipe);

    // Wait for a brief period to allow the service to initialize
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
}

void test_version_endpoint(Results& r, httplib::Client& cli) {
    if (auto res = cli.Get("/api/version")) {
        r.equals(res->status == 200, "version endpoint status should be 200");
        fmt::print("\t{}Test passed: Version endpoint returned correct response{}\n", green, reset);
    } else {
        r.fail("version endpoint status should not 200");
        fmt::print("\t{}Test failed: Unable to reach version endpoint.{}\n", red, reset);
    }
}

void test_index_page_endpoint(Results& r, httplib::Client& cli) {
    if (auto res = cli.Get("/")) {
        r.equals(res->status == 200);
        r.equals(res->body.find("<title>") != Str::npos, "the plain home page should exist.");
        fmt::print("\t{}Test passed: Index page contains a title.{}\n", green, reset);
    } else {
        r.fail("index page failed");
        fmt::print("\t{}Test failed: Unable to reach index page.{}\n", red, reset);
    }
}

void test_put_temperature_endpoint(Results& r, httplib::Client& cli) {
    auto now = datetimelib::timestamp_seconds();
    Str key = fmt::format("{}.test-location", now);
    float value = 10.1234;

    nlohmann::json jdata = {{"key", key}, {"value", value}};
    auto body = jdata.dump();

    if (auto res = cli.Put("/api/temperature", body, "application/json")) {
        if (res->status == 200) {
            r.equals(res->status == 200, "put data should return 200");
            fmt::print("\t{}Test passed: Put temperature data.{}\n", green, reset);
        } else {
            r.fail("put data failed: " + body);
            fmt::print("\t{}Test failed: Unable to put temperature data. {} {}\n", red, body,
                       reset);
            fmt::print("{}body: {}{}\n", yellow, res->body, reset);
        }
    } else {
        auto err = httplib::to_string(res.error());
        r.fail("failed: " + err + ", body: " + body);
        fmt::print("\t{}Test failed: Unable to put temperature data: {}{}\n", err, red, reset);
    }
}

void test_shutdown_endpoint(Results& r, httplib::Client& cli) {
    if (auto res = cli.Delete("/api/shutdown")) {
        r.equals(res->status == 200, "shutdown return status should be 200");
        r.equals(res->body.find("down") != Str::npos, "the response should say down");
        fmt::print("\t{}Test passed: Shutdown endpoint had correct response.{}\n", green, reset);
    } else {
        r.fail("server did not shutdown");
        fmt::print("\t{}Test failed: Unable to reach shutdown endpoint.{}\n", red, reset);
    }
}

void test_database(Results& r, app::database::Database& db) {
    app::database::read_current_data(db);

    r.equals(db.size() > 0, "database size");
    fmt::print("\t{}Test passed: database size: {}{}\n", green, db.size(), reset);

    r.equals(db.keys().size() == db.size(), "database size matches key size");
    fmt::print("\t{}Test passed: database keys size: {}{}\n", green, db.size(), reset);
}

// the main test suite
int main() {
    std::atomic<bool> server_running(false);
    auto config = Config();

    app::database::Database db;

    Str msg = "DataLogger Integration Tests, Version: ";
    fmt::print("\n{}{}{}{}{}\n", cyan, msg, yellow, app::Version().to_string(), reset);

    Results r = {.name = "Integration Test Summary"};

    // start the server thread
    std::thread server_thread(run_server, std::ref(server_running), config);

    // Wait for the server to start
    auto loop_count = 25;
    while (!server_running && loop_count > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        loop_count--;
    }

    r.equals(server_running, "should be running in background thread now");

    fmt::print("\n");

    // Create a client for testing
    httplib::Client cli(config.get_url());
    // cli.enable_server_certificate_verification(false);

    //
    // Run the Tests
    //
    test_database(r, db);
    test_version_endpoint(r, cli);
    test_index_page_endpoint(r, cli);
    test_put_temperature_endpoint(r, cli);
    test_shutdown_endpoint(r, cli);

    // Wait for the server thread to stop
    server_thread.join();

    // give the service time to fully shutdown
    std::this_thread::sleep_for(std::chrono::milliseconds(600));

    // Verify server has stopped
    // try to Shut down the server
    if (auto res = cli.Delete("/api/shutdown")) {
        r.fail("server should be shutdown here");
        fmt::print("\t{}Test failed: Unable to reach shutdown endpoint.{}\n", red, reset);
    } else {
        r.equals(true, "shutdown ok");
        fmt::print("\t{}Test passed: the server is down.{}\n", green, reset);
        server_running = false;
    }

    r.equals(!server_running, "server should NOT be running.");

    fmt::print("\n{}{}{}\n", cyan, r.to_string(), reset);
    msg = (r.failed == 0) ? green + "All Tests Passed, Ok" : "\n" + red + "Tests failed!";

    fmt::print("\n{}Integration Test Results: {}{}\n", cyan, msg, reset);

    return 0;
}
