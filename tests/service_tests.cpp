//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/types.hpp>
#include <app/service.hpp>

TEST_CASE("Service Tests", "[service][setup_service]") {
    // TODO setup service
    const Str ss = "test string";
    REQUIRE(ss == "test string");
}
