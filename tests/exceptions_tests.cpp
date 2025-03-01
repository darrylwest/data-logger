//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/types.hpp>
#include <app/exceptions.hpp>

bool bad_http_get(const Str& url) {
    if (url == "http://bad_network.com") {
        throw app::NetworkException("Failed to connect to url: " + url);
    } else if (url == "http://service_down.com") {
        throw app::ServiceException("Service Failed for url: " + url);
    } else if (url == "http://bad_parse.com") {
        throw app::ParseException("JSON parse failed at url: " + url);
    }

    // return false; should never get here
    return false;
}

bool bad_db_file(const Str& filename) {
    if (filename == "bad_db_file") {
        throw app::DatabaseException("Db Failed to connect to file: " + filename);
    } else if (filename == "bad_filename") {
        throw app::FileException("Failed to open: " + filename);
    }

    // return false; should never get here
    return false;
}

TEST_CASE("Exceptions Tests", "[exceptions][network]") {
    try {
        bad_http_get("http://bad_network.com");
        INFO("should not get here from a bad http get");
        REQUIRE(false);
    } catch (const std::exception& e) {
        spdlog::info("exception: {}", e.what());
        REQUIRE(true);
    }
}

TEST_CASE("Exceptions Tests", "[exceptions][service]") {
    try {
        bad_http_get("http://service_down.com");
        INFO("should not get here from service down");
        REQUIRE(false);
    } catch (const std::exception& e) {
        spdlog::info("exception: {}", e.what());
        REQUIRE(true);
    }
}

TEST_CASE("Exceptions Tests", "[exceptions][parse]") {
    try {
        bad_http_get("http://bad_parse.com");
        INFO("should not get here from a bad http parse");
        REQUIRE(false);
    } catch (const std::exception& e) {
        spdlog::info("exception: {}", e.what());
        REQUIRE(true);
    }
}

TEST_CASE("Exceptions Tests", "[exceptions][db-file]") {
    try {
        bool ok = bad_db_file("bad_db_file");
        INFO("should not get here from a bad db file");
        REQUIRE(ok);
    } catch (const std::exception& e) {
        spdlog::info("exception: {}", e.what());
        REQUIRE(true);
    }
}
