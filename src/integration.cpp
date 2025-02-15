//
// 2024-12-23 15:25:10 dpw
//

#include <httplib.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <unistd.h>

#include <app/version.hpp>
#include <atomic>
#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <vendor/ansi_colors.hpp>
#include <vendor/testlib.hpp>

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

    fmt::println("{}Server Start Command: {}{}{}", cyan, yellow, cmd, reset);

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        fmt::println("{}File to start service.{}", red, reset);
        running = false;
        return;
    }

    // Read the process ID of the started service
    char buffer[128];
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        pid_t pid = std::stoi(buffer);
        fmt::println("{}Service started with PID: {}{}", cyan, yellow, pid, reset);
    } else {
        fmt::println("\t{}Failed to retrieve service PID!", red, reset);
        running = false;
    }

    pclose(pipe);

    // Wait for a brief period to allow the service to initialize
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
}

void test_version_endpoint(Results& r, httplib::Client& cli) {
    if (auto res = cli.Get("/version")) {
        r.equals(res->status == 200, "version endpoint status should be 200");
        fmt::println("\t{}Test passed: Version endpoint returned correct response{}", green, reset);
    } else {
        r.fail("version endpoint status should not 200");
        fmt::println("\t{}Test failed: Unable to reach version endpoint.{}", red, reset);
    }
}

void test_index_page_endpoint(Results& r, httplib::Client& cli) {
    if (auto res = cli.Get("/")) {
        r.equals(res->status == 200);
        r.equals(res->body.find("<title>") != Str::npos, "the plain home page should exist.");
        fmt::println("\t{}Test passed: Index page contains a title.", green, reset);
    } else {
        r.fail("index page failed");
        fmt::println("\t{}Test failed: Unable to reach index page.{}", red, reset);
    }
}

// void test_temperature_put_endpoint(Results& r, httplib::Client& cli) {
// }

void test_shutdown_endpoint(Results& r, httplib::Client& cli) {
    if (auto res = cli.Delete("/shutdown")) {
        r.equals(res->status == 200, "return status should be 200");
        r.equals(res->body.find("down") != Str::npos, "the response should say down");
        fmt::println("\t{}Test passed: Shutdown endpoint had correct response.{}", green, reset);
    } else {
        r.fail("server did not shutdown");
        fmt::println("\t{}Test failed: Unable to reach shutdown endpoint.", red, reset);
    }
}

// the main test suite
int main() {
    std::atomic<bool> server_running(false);
    auto config = Config();

    Str msg = "DataLogger Integration Tests, Version: ";
    fmt::println("\n{}{}{}{}{}", cyan, msg, yellow, app::Version().to_string(), reset);

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

    fmt::println("");

    // Create a client for testing
    httplib::Client cli(config.get_url());
    cli.enable_server_certificate_verification(false);

    //
    // Run the Tests
    //
    test_version_endpoint(r, cli);
    test_index_page_endpoint(r, cli);
    // text_temperature_put_endpoint(r, cli);
    test_shutdown_endpoint(r, cli);

    // Wait for the server thread to stop
    server_thread.join();

    // give the service time to fully shutdown
    std::this_thread::sleep_for(std::chrono::milliseconds(600));

    // Verify server has stopped
    // try to Shut down the server
    if (auto res = cli.Delete("/shutdown")) {
        r.fail("server should be shutdown here");
        fmt::println("\t{}Test failed: Unable to reach shutdown endpoint.{}", red, reset);
    } else {
        r.equals(true, "shutdown ok");
        fmt::println("\t{}Test passed: the server is down.{}", green, reset);
        server_running = false;
    }

    r.equals(!server_running, "server should NOT be running.");

    fmt::println("\n{}{}{}", cyan, r.to_string(), reset);
    msg = (r.failed == 0) ? green + "All Tests Passed, Ok" : "\n" + red + "Tests failed!";

    fmt::println("\n{}Integration Test Results: {}{}", cyan, msg, reset);

    return 0;
}
