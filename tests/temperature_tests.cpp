//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/temperature.hpp>
#include <app/types.hpp>
#include "test_helpers.hpp"

using namespace app;

struct TemperatureTestSetup {
    TemperatureTestSetup() {
        spdlog::set_level(spdlog::level::err);
    }

    ~TemperatureTestSetup() {
        spdlog::set_level(spdlog::level::off);
    }
};

TEST_CASE_METHOD(TemperatureTestSetup, "Temperature Tests", "[temperature][c2f]") {
    Vec<float> values = {0.0, 10.0, 15.0, 20.0, 25.0, 30.0};

    for (auto value : values) {
        auto f = temperature::celsius_to_fahrenheit(value);
        auto g = (value * temperature::C2F_RATIO) + temperature::C2F_OFFSET;

        REQUIRE(std::abs(g - f) < helpers::EPSILON);
    }
}

TEST_CASE_METHOD(TemperatureTestSetup, "Temperature Tests", "[temperature][parse_reading]") {
    const auto reading = helpers::mock_reading;
    const temperature::TemperatureData data = temperature::parse_reading(reading);

    REQUIRE(data.reading_at == 1738362466);
    REQUIRE(data.probes.size() == 2);

    auto probe0 = data.probes.at(0);
    REQUIRE(probe0.sensor == 0);
    REQUIRE(probe0.location == "cottage-south");
    REQUIRE(probe0.enabled);
    REQUIRE(std::abs(probe0.tempC - 10.88542) < helpers::EPSILON);
    REQUIRE(std::abs(probe0.tempF - 51.59375) < helpers::EPSILON);

    auto probe1 = data.probes.at(1);
    REQUIRE(probe1.sensor == 1);
    REQUIRE(probe1.location == "cottage-east");
    REQUIRE(probe1.enabled == false);
    REQUIRE(probe1.tempC < 100);
    REQUIRE(probe1.tempF < 100);

}

TEST_CASE_METHOD(TemperatureTestSetup, "Temperature Tests", "[temperature][parse_probe]") {
    const auto text = helpers::mock_client_config;
    json jcfg = json::parse(text);
    auto location = "cottage-south";

    auto probe = temperature::parse_probe(jcfg, location);
    REQUIRE(probe.has_value());
    REQUIRE(probe->location == location);
    REQUIRE(probe->sensor == 0);
    REQUIRE(probe->enabled);
}

