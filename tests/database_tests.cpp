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
        spdlog::set_level(spdlog::level::critical);
    }

    ~DatabaseTestSetup() {
        spdlog::set_level(spdlog::level::off);
    }
};

void populate_database(database::Database& db, const int size = 100) {
    time_t t0 = datetimelib::timestamp_seconds() - (size * 10);
    Str location = "shed.0";

    for (int i = 0; i < size; ++i) {
        auto key = database::create_key(t0, location);
        auto value = helpers::random_float(20.0, 30.0);

        db.set(key.to_string(), std::to_string(value));

        t0 += 10;
    }
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[client][set_get]") {
    // TODO implement
    REQUIRE(true);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[client][keys]") {
    // TODO implement
    REQUIRE(true);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[client][last_n]") {
    // TODO implement
    REQUIRE(true);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[client][search]") {
    // TODO implement
    REQUIRE(true);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][read_database]") {
    // write out a raw database file key/value
    spdlog::info("raw data: {}", helpers::raw_temps_data);
    const auto path = helpers::create_temp_path("db-read-test_");

    spdlog::info("write to: {}", path.string());

    std::ofstream os(path);
    os << helpers::raw_temps_data;
    os.close();

    database::Database db;
    REQUIRE(db.size() == 0);

    bool ok = db.read(path);
    REQUIRE(ok);
    REQUIRE(db.size() == 12);

    for (const auto& key : db.keys()) {
        auto value = db.get(key);
        float v = std::stof(value);
        REQUIRE(v < 17.0);
        REQUIRE(v > 15.0);
    }

    helpers::remove_temp_path(path);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][write_database]") {
    database::Database db;
    REQUIRE(db.size() == 0);
    size_t size = 20;
    populate_database(db, size);
    REQUIRE(db.size() == size);

    const auto path = helpers::create_temp_path("db-write-test_");
    bool ok = db.save(path);
    REQUIRE(ok);

    database::Database rdb;
    rdb.read(path);
    REQUIRE(rdb.size() == db.size());

    helpers::remove_temp_path(path);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][create_key]") {

    time_t timestamp = 1739563051;
    const auto location = "shed.0";
    database::DbKey key = database::create_key(timestamp, location);

    REQUIRE(key.location == location);
    REQUIRE(key.timestamp == timestamp);
    REQUIRE(key.to_string() == "1739563051.shed.0");
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][append_key_value]") {

    const auto path = helpers::create_temp_path();
    const time_t ts = datetimelib::timestamp_seconds();
    const auto key = app::database::create_key(ts, "tmp.0");

    const auto value = helpers::random_float(-10.0, 40.0);
    const Str sval = std::to_string(value);
    spdlog::info("file: {} {} {}", path.c_str(), key.to_string(), sval);

    INFO("will throw if this operation failed");

    try {
        append_key_value(path, key, sval);
        REQUIRE(true);
    } catch (const std::exception& e) {
        spdlog::error("{}", e.what());
        REQUIRE(false);
    }

    helpers::remove_temp_path(path);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][bad_append_file]") {

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
        spdlog::error("{}", e.what());}
        REQUIRE(true);
}

TEST_CASE_METHOD(DatabaseTestSetup, "Database Tests", "[database][data]") {
    // create some data and store in a temp file
    database::Database db;
    REQUIRE(db.size() == 0);

    const size_t size = 250;
    populate_database(db, size);
    // open the database and read the file
    REQUIRE(db.size() == size);

    auto keys = db.keys();

    Str key = keys.at(25);
    REQUIRE(key.size() > 6);
    Str value = db.get(key);
    REQUIRE(value.size() > 3);
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

