#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() function
#include <catch2/catch_all.hpp>  // For Catch2 v3

TEST_CASE("Version test", "[version]") {
    REQUIRE(1 + 1 == 2);
    REQUIRE(3 + 3 == 6);
}
