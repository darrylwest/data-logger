#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() function
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include <app/version.hpp>

TEST_CASE("Version test", "[version]") {

    auto vers = app::Version();

    INFO("Checking Application Version: " << vers.to_string());

    REQUIRE(vers.major == 0);
    REQUIRE(vers.minor == 6);
    REQUIRE(vers.patch == 5);
    REQUIRE(vers.build > 200);
}
