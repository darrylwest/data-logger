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
// namespace app;

struct Config {
    std::string host = "localhost";
    std::string port = "29099";
    bool start_server = true;
    std::string logfile = "logs/integration-test.log";
};

// Define the function to start the service
void run_server(std::atomic<bool>& running, const Config& config) {
    running = true;

    std::string cmd = fmt::format("./build/data-logger --host {} --port {} > {} 2>&1 & echo $!",
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
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
}

// use cxxopts to parse host and port
// if the service is not local (localhost or 127.0.0.1), then skip the startup
int main(int argc, char* argv[]) {
    std::atomic<bool> server_running(false);
    auto config = Config();

    // TODO parse the cli to reconfig...

    std::string msg = "DataLogger Integration Tests, Version: ";
    fmt::println("\n{}{}{}{}{}", cyan, msg, yellow, app::Version().to_string(), reset);

    rcstestlib::Results r = {.name = "Integration Test Summary"};

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
    httplib::Client cli("https://" + config.host + ":" + config.port);
    cli.enable_server_certificate_verification(false);

    // Test 1: Verify version endpoint
    if (auto res = cli.Get("/version")) {
        r.equals(res->status == 200, "the status should be 200");
        fmt::println("\t{}Test passed: Version endpoint returned correct response{}", green, reset);
    } else {
        fmt::println("\t{}Test failed: Unable to reach version endpoint.{}", red, reset);
    }

    // Verify index page title
    if (auto res = cli.Get("/")) {
        r.equals(res->status == 200);
        r.equals(res->body.find("<title>") != std::string::npos,
                 "the plain home page should exist.");
        fmt::println("\t{}Test passed: Index page contains a title.", green, reset);
    } else {
        fmt::println("\t{}Test failed: Unable to reach index page.{}", red, reset);
    }

    // Shut down the server
    if (auto res = cli.Delete("/shutdown")) {
        r.equals(res->status == 200, "return status should be 200");
        r.equals(res->body.find("down") != std::string::npos, "the response should say down");
        fmt::println("\t{}Test passed: Shutdown endpoint had correct response.{}", green, reset);
    } else {
        fmt::println("\t{}Test failed: Unable to reach shutdown endpoint.", red, reset);
    }

    // Wait for the server thread to stop
    server_thread.join();

    // give the service time to fully shutdown
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Verify server has stopped
    // try to Shut down the server
    if (auto res = cli.Delete("/shutdown")) {
        r.equals(res->status != 200, "should be shutdown");
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
