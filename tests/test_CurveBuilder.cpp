//
// Created by suhasghorp on 8/16/2021.
//

#include "../src/QLCurveBuilder.h"
#include "../src/QLDynamoDBHelper.h"
#include <catch.hpp>
#include <ql/indexes/ibor/usdlibor.hpp>
#include <ql/instruments/forwardrateagreement.hpp>
#include <ql/instruments/makevanillaswap.hpp>
#include <ql/pricingengines/swap/discountingswapengine.hpp>
#include <ql/time/daycounters/thirty360.hpp>
#include <boost/compute/detail/lru_cache.hpp>

using namespace QuantLib;

TEST_CASE("curvebuilder build test") {
  static boost::compute::detail::lru_cache<std::string, std::string> cache(10);
  Date today(16, August,2021);
  Calendar calendar = UnitedStates();
  Date settlement = calendar.advance(today,2,Days);
  //QLCurveBuilder builder("2021-08-16", "USD");
  /*std::shared_ptr<YieldTermStructure> yieldCurve = builder.build();*/

  json discRates, forecastRates, conventions;
  const auto cacheDiscKey = "2021-08-16|SOFR";
  boost::optional<std::string> maybeRates = cache.get(cacheDiscKey);
  if (maybeRates) {//sofr rates were found in the cache
    std::cout << "discount rates were found in the cache" << std::endl;
    discRates = json::parse(maybeRates.get());
  } else {
    std::cout << "discount rates were not in the cache" << std::endl;
    discRates = dynamo::getRiskFreeRates("2021-08-16", "USD");
    cache.insert(cacheDiscKey, discRates.dump());
  }
  const auto cacheForecastKey = "2021-08-16|USD-LIBOR-3M";
  maybeRates = cache.get(cacheForecastKey);
  if (maybeRates) {//libor rates were found in the cache
    std::cout << "forecast rates were found in the cache" << std::endl;
    forecastRates = json::parse(maybeRates.get());
  } else {
    std::cout << "forecast rates were not in the cache" << std::endl;
    conventions = dynamo::getLiborRates("2021-08-16", "USD");
    cache.insert(cacheForecastKey, forecastRates.dump());
  }

  const auto convKey = "USD-CONVENTIONS";
  maybeRates = cache.get(convKey);
  if (maybeRates) {//conventions were found in the cache
    std::cout << "conventions were found in the cache" << std::endl;
    conventions = json::parse(maybeRates.get());
  } else {
    std::cout << "conventions were not in the cache" << std::endl;
    conventions = dynamo::getConventions("USD");
    cache.insert(convKey, conventions.dump());
  }



  QLCurveBuilder builder;

  RelinkableHandle<YieldTermStructure> discountingTermStructure;
  std::shared_ptr<YieldTermStructure> discountCurve = builder.buildDiscountCurve("2021-08-16", "USD", true, discRates);
  discountingTermStructure.linkTo(discountCurve);

  RelinkableHandle<YieldTermStructure> forecastingTermStructure;
  std::shared_ptr<YieldTermStructure> yieldCurve = builder.buildProjectionCurve("2021-08-16", "USD", true, forecastRates, conventions, discountCurve);
  forecastingTermStructure.linkTo(yieldCurve);

  USDLibor usdLibor(Period(3*Months),forecastingTermStructure);
  double estimatedRate = usdLibor.fixing(today);
  double expectedRate = 0.1245/100;
  REQUIRE (fabs(estimatedRate - expectedRate) < 0.001);

  std::shared_ptr<IborIndex> libor3m(new USDLibor(Period(3,Months),forecastingTermStructure));

  //futures
  Date immStart = Date(15, December,2021);
  Date end = calendar.advance(immStart, 3, Months,
                                   libor3m->businessDayConvention(),
                                   libor3m->endOfMonth());

  ForwardRateAgreement immFut(immStart, end, Position::Long,
                              (100 - 99.8228)/100, 100.0,
                              libor3m, forecastingTermStructure);
  expectedRate = (100 - 99.8228)/100;
  estimatedRate = immFut.forwardRate();
  REQUIRE (fabs(estimatedRate - expectedRate) < 0.001);

  //swap
  VanillaSwap swap = MakeVanillaSwap(Period(8,Years),libor3m, 0.0)
      .withEffectiveDate(settlement)
      .withFixedLegDayCount(Thirty360(Thirty360::Convention::BondBasis))
      .withFixedLegTenor(Period(Semiannual))
      .withFixedLegConvention(ModifiedFollowing)
      .withFixedLegTerminationDateConvention(ModifiedFollowing);

  estimatedRate = swap.fairRate();
  std::shared_ptr<PricingEngine> swapEngine(new DiscountingSwapEngine(discountingTermStructure));
  swap.setPricingEngine(swapEngine);
  std::cout << swap.NPV() << std::endl;
  expectedRate = 1.1326/100.0;
  REQUIRE (fabs(estimatedRate - expectedRate) < 0.001);

  
}