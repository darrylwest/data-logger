//
// Created by Darryl West on 2/27/25.
//
#include <app/database.hpp>
#include <app/types.hpp>
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <filesystem>

#include "test_helpers.hpp"

using namespace app;

struct DatabaseTestSetup {
    DatabaseTestSetup() {
        spdlog::set_level(spdlog::level::info);
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
    DatabaseTestSetup setup;

    const std::filesystem::path path = "/tmp/test-append.db";
    const time_t ts = datetimelib::timestamp_seconds();
    const auto key = app::database::create_key(ts, "tmp.0");

    const auto value = helpers::random_float(-10.0, 40.0);
    const Str sval = std::to_string(value);
    spdlog::info("file: {} {} {}", path.c_str(), key.to_string(), sval);

    INFO("will throw if this operation failed");

    try {
        database::append_key_value(path, key, sval);
        REQUIRE(true);
    } catch (const std::exception& e) {
        REQUIRE(false);
    }

    helpers::remove_temp_path(path);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][bad_append_file]") {
    DatabaseTestSetup setup;

    const auto filename = "bad-file/folder/temps/bad.db";
    const time_t ts = datetimelib::timestamp_seconds();
    const auto key = app::database::create_key(ts, "bad.0");
    const auto sval = "22.3344";

    spdlog::info("file: {} {} {}", filename, key.to_string(), sval);

    INFO("will throw if this operation failed");

    try {
        database::append_key_value(filename, key, sval);
        REQUIRE(false);
    } catch (const std::exception& e) {
        REQUIRE(true);
    }
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][data]") {
    REQUIRE(1 == 1);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][read_data]") {
    const FilePath path = helpers::create_temp_path("tmpdb_");
    spdlog::info("file: {}", path.string());
    REQUIRE(1 == 1);

    helpers::remove_temp_path(path);
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

