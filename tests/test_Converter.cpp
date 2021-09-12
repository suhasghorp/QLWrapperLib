//
// Created by suhasghorp on 8/16/2021.
//
#include "../src/QLConverter.h"
#include <catch.hpp>

using namespace std;

TEST_CASE("toDate test") {
  Date one(10, March, 2008);
  auto two = to_date("2008-03-10");
  REQUIRE (one.serialNumber() == two.serialNumber());

}

TEST_CASE("to_bus_day_convention test") {
  BusinessDayConvention one = ModifiedFollowing;
  auto two = to_bus_day_convention("ModifiedFollowing");
  REQUIRE (one == two);
}

TEST_CASE("to_calendar test") {
  Calendar one = TARGET();
  Calendar two = to_calendar("Target");
  REQUIRE (one.name() == two.name());
}

TEST_CASE("to_swap_type test") {
  Swap::Type one = Swap::Receiver;
  Swap::Type two = to_swap_type("Receiver");
  REQUIRE (one == two);
}

TEST_CASE("to_frequency test") {
  Frequency one = Monthly;
  Frequency two = to_frequency("Monthly");
  REQUIRE (one == two);
}

TEST_CASE("toDateGenRule test") {
  DateGeneration::Rule one = DateGeneration::Backward;
  DateGeneration::Rule two = to_date_gen_rule("Backward");
  REQUIRE (one == two);
}

TEST_CASE("to_day_counter test") {
  DayCounter one = Actual360();
  DayCounter two = to_day_counter("ACTUAL360");
  REQUIRE (one.name() == two.name());
}

TEST_CASE("to_ibor_index test") {
  std::shared_ptr<IborIndex> one = std::make_shared<Euribor>(Euribor(6 * Months));
  std::shared_ptr<IborIndex> two = to_ibor_index("EUR.6M");
  REQUIRE(((one->currency() == two->currency()) && (one->tenor().length() == two->tenor().length())));
}












