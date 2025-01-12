
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/version.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main(int argc, char *argv[]) {
    std::ifstream f("config/config.json");
    json data = json::parse(f);
    std::cout << data << '\n';

    const auto vers = app::Version();

    auto config = app::parse_cli(argc, argv);
    spdlog::info("Example Version: {}", vers.to_string());

    return 0;
}
