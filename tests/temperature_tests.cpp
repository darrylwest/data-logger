//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include <app/types.hpp>
#include <app/temperature.hpp>

TEST_CASE("Temperature Tests", "[temperature]") {
    const Str ss = "test string";
    REQUIRE(ss == "test string");
}