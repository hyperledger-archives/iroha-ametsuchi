#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <ametsuchi/Greeter.h>

TEST_CASE("Greeter greets the proper subject", "[greeter]") {
  ametsuchi::Greeter greeter("Iroha");

  REQUIRE(greeter.greet() == "Hello, Iroha!");
}
