//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/types.hpp>
#include <app/database.hpp>
#include "test_helpers.hpp"

using namespace app;

struct DatabaseTestSetup {
    DatabaseTestSetup() {
        spdlog::set_level(spdlog::level::err);
    }

    ~DatabaseTestSetup() {
        spdlog::set_level(spdlog::level::off);
    }
};


TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][create_key]") {
    time_t timestamp = 1739563051;
    const auto location = "shed.0";
    database::DbKey key = database::create_key(timestamp, location);

    REQUIRE(key.location == location);
    REQUIRE(key.timestamp == timestamp);
    REQUIRE(key.to_string() == "1739563051.shed.0");
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][append_key_value]") {
    REQUIRE(1 == 1);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][data]") {
    REQUIRE(1 == 1);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][read_current]") {
    REQUIRE(1 == 1);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][reading_types]") {
    using namespace app::database;
    Str label = ReadingType::to_label(ReadingType::Value::Temperature);
    REQUIRE(label == "temperature");
    label = ReadingType::to_label(ReadingType::Value::Status);
    REQUIRE(label == "status");
    label = ReadingType::to_label(ReadingType::Value::Distance);
    REQUIRE(label == "distance");
    label = ReadingType::to_label(ReadingType::Value::Light);
    REQUIRE(label == "light");
    label = ReadingType::to_label(ReadingType::Value::Humidity);
    REQUIRE(label == "humidity");
    label = ReadingType::to_label(ReadingType::Value::Proximity);
    REQUIRE(label == "proximity");
}

