//
// Created by Darryl West on 2/27/25.
//
#include <app/database.hpp>
#include <app/types.hpp>
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <filesystem>
#include <datetimelib/perftimer.hpp>

#include "test_helpers.hpp"

using namespace app;
using namespace datetimelib::perftimer;

void populate_database(database::Database& db, const size_t size = 100) {
    time_t t0 = datetimelib::timestamp_seconds() - (size * 10);
    Str location = "shed.0";

    for (size_t i = 0; i < size; ++i) {
        auto key = database::create_key(t0, location);
        auto value = helpers::random_float(20.0, 30.0);

        db.set(key.to_string(), std::to_string(value));

        t0 += 10;
    }
}

TEST_CASE("Database Tests", "[client][set_get]") {
    database::Database db;
    REQUIRE(db.size() == 0);
    size_t size = 10;
    populate_database(db, size);
    REQUIRE(db.size() == size);

    auto keys = db.keys();

    for (const auto& key : keys) {
        INFO("find for key: " << key);
        auto ret = db.get(key);
        if (ret) {
            REQUIRE(true);
        } else {
            REQUIRE(false);
            continue;
        }

        const auto value = *ret;

        auto new_value = helpers::random_float(30.0, 40.0);
        auto resp = db.set(key, std::to_string(new_value));
        INFO("replacing/updating a value should always return false");
        REQUIRE(resp == false);
        ret = db.get(key);
        if (ret) {
            REQUIRE(true);
        } else {
            REQUIRE(false);
            continue;
        }

        const auto return_value = *ret;
        REQUIRE(return_value == std::to_string(new_value));
        REQUIRE(std::to_string(new_value) != value);
    }
    REQUIRE(true);
}

TEST_CASE("Database Tests", "[client][get_bad_key]") {
    database::Database db;
    REQUIRE(db.size() == 0);
    size_t size = 5;
    populate_database(db, size);
    REQUIRE(db.size() == size);

    auto bad_key = "not-a-good-key";
    auto value = db.get(bad_key);
    REQUIRE(!value);

    auto key = db.keys().at(0);
    value = db.get(key);
    REQUIRE(value);

    // now delete the k/v and ensure that a read-back is empty
    db.remove(key);
    value = db.get(key);
    REQUIRE(!value);
}

TEST_CASE("Database Tests", "[client][keys]") {
    database::Database db;
    REQUIRE(db.size() == 0);
    size_t size = 50;
    populate_database(db, size);
    REQUIRE(db.size() == size);
    auto keys = db.keys();
    REQUIRE(keys.size() == size);

    for (const auto& key : keys) {
        auto value = db.get(key);
        REQUIRE(value);
    }
}

TEST_CASE("Database Tests", "[client][last_n]") {
    // TODO implement
    REQUIRE(true);
}

TEST_CASE("Database Tests", "[client][search]") {
    // TODO implement
    REQUIRE(true);
}

TEST_CASE("Database Tests", "[database][read_database]") {
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
        if (value) {
            float v = std::stof(*value);
            REQUIRE(v < 17.0);
            REQUIRE(v > 15.0);
        } else {
            REQUIRE(false);
        }
    }

    helpers::remove_temp_path(path);
}

TEST_CASE("Database Tests", "[database][write_database]") {
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

TEST_CASE("Database Tests", "[database][create_key]") {

    time_t timestamp = 1739563051;
    const auto location = "shed.0";
    database::DbKey key = database::create_key(timestamp, location);

    REQUIRE(key.location == location);
    REQUIRE(key.timestamp == timestamp);
    REQUIRE(key.to_string() == "1739563051.shed.0");
}

TEST_CASE("Database Tests", "[database][append_key_value]") {

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

TEST_CASE("Database Tests", "[database][bad_append_file]") {

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

TEST_CASE("Database Tests", "[database][data]") {
    // create some data and store in a temp file
    database::Database db;
    REQUIRE(db.size() == 0);

    const size_t size = 250;
    populate_database(db, size);
    // open the database and read the file
    REQUIRE(db.size() == size);

    auto keys = db.keys();

    const auto key = keys.at(25);
    REQUIRE(key.size() > 6);
    auto ret = db.get(key);
    if (ret) {
        const auto value = *ret;
        REQUIRE(value.size() > 3);
    } else {
        REQUIRE(false);
    }
}

TEST_CASE("Database Tests", "[database][read_data]") {
    const FilePath path = helpers::create_temp_path("tmpdb_");
    spdlog::info("file: {}", path.string());
    REQUIRE(1 == 1);

    helpers::remove_temp_path(path);
}

TEST_CASE("Database Tests", "[database][reading_types]") {
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
