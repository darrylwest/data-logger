#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() function
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include <app/version.hpp>
#include <app/types.hpp>

TEST_CASE("Version test", "[version]") {

    const auto vers = app::Version();

    INFO("Checking Application Version: " << vers.to_string());

    REQUIRE(vers.major == 0);
    REQUIRE(vers.minor == 6);
    REQUIRE(vers.patch == 5);
    REQUIRE(vers.build > 200);

    const Str ss = vers.to_string();

    REQUIRE(ss.starts_with("0.6."));
}
