//
// Created by suhasghorp on 8/21/2021.
//
#include <catch.hpp>
#include <iostream>
#include <json/json.hpp>
#include <QLWrapperLib/QLSwapPricer.h>
#include <boost/compute/detail/lru_cache.hpp>

using json = nlohmann::json;

TEST_CASE("curvebuilder build test") {
  /*json temp = R"({"valDate": "2021-08-16", "currency":"USD", "swapType": "PAYER", "nominal": 160000000.0, "startDate": "2018-03-14", "maturityDate": "2028-03-14", "fixedLegFrequency": "SEMIANNUAL",
  "fixedLegCalendar": "UNITEDSTATES", "fixedLegConvention": "MODIFIEDFOLLOWING", "fixedLegDateGenerationRule": "BACKWARD", "fixedLegRate": 0.02,
  "fixedLegDayCount": "ACTUAL360", "fixedLegEndOfMonth": false, "floatingLegFrequency": "QUARTERLY", "floatingLegCalendar": "UNITEDSTATES",
  "floatingLegConvention": "MODIFIEDFOLLOWING", "floatingLegDateGenerationRule": "BACKWARD", "floatingLegSpread": 0.0007, "floatingLegDayCount": "ACTUAL360",
"floatingLegEndOfMonth": false})"_json;*/

  //calypso id 491159

  json temp = R"({"valDate": "2021-08-16", "currency":"USD", "swapType": "PAYER", "nominal": 46860000.0, "startDate": "2014-04-02", "maturityDate": "2024-04-02", "fixedLegFrequency": "SEMIANNUAL",
"fixedLegCalendar": "UNITEDSTATES", "fixedLegConvention": "MODIFIEDFOLLOWING", "fixedLegDateGenerationRule": "BACKWARD", "fixedLegRate": 0.0285,
"fixedLegDayCount": "ACTUAL360", "fixedLegEndOfMonth": false, "floatingLegFrequency": "QUARTERLY", "floatingLegCalendar": "UNITEDSTATES",
"floatingLegConvention": "MODIFIEDFOLLOWING", "floatingLegDateGenerationRule": "BACKWARD", "floatingLegSpread": 0.0, "floatingLegDayCount": "ACTUAL360", "floatingLegEndOfMonth": false})"_json;
  boost::compute::detail::lru_cache<std::string, std::string> cache(10);
  std::string response = priceSwap(temp.dump(), cache);
  std::cout << response << std::endl;
}
