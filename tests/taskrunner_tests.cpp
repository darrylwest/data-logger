#include <catch2/catch_all.hpp>  // For Catch2 v3

#include <app/types.hpp>

TEST_CASE("Task Runner Tests", "[taskrunner]") {
    const Str ss = "test string";
    REQUIRE(ss == "test string");
}
