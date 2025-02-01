
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/version.hpp>

int main(int argc, char *argv[]) {
    const auto vers = app::Version();

    auto config = app::parse_cli(argc, argv);
    spdlog::info("DataLogger Version: {}", vers.to_string());

    // start the ticker + jobs

    // now start the listener
    auto ok = app::run_service(config);
    spdlog::info("Server shutdown, code: {}.", ok);

    return 0;
}
