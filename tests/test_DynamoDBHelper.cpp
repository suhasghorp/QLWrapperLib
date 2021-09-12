//
// Created by suhasghorp on 8/17/2021.
//

#include "../src/QLDynamoDBHelper.h"
#include <catch.hpp>

TEST_CASE("getLiborRates test") {
  auto str = dynamo::getLiborRates("2021-08-16", "USD");
  std::cout << str << std::endl;
}

TEST_CASE("getConventions test") {
  auto str = dynamo::getConventions("USD");
  std::cout << str << std::endl;
}

TEST_CASE("getFixing test") {
  double fixing = dynamo::getFixing("usd-libor-3m", "2021-06-10");
  std::cout << fixing << std::endl;
}
