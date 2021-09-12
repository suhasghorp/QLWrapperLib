//
// Created by suhasghorp on 8/28/2021.
//
//#include "../src/QLCurveBuilder.h"
#include <catch.hpp>
#include <ql/indexes/ibor/sofr.hpp>
#include <ql/indexes/ibor/usdlibor.hpp>
#include <ql/instruments/forwardrateagreement.hpp>
#include <ql/instruments/makevanillaswap.hpp>
#include <ql/termstructures/yield/ratehelpers.hpp>
#include <ql/utilities/dataparsers.hpp>
#include <string>
#include <iostream>

using namespace QuantLib;

TEST_CASE("sofr curvebuilder build test") {

  Settings::instance().evaluationDate() = Date(16,August,2021);

  std::vector<std::string> periods {"1M","2M","3M","4M","5M","6M","9M","1Y",
                                   "18M","2Y","3Y","4Y","5Y","7Y","10Y","12Y",
                                   "15Y","20Y","25Y","30Y","40Y","50Y"};
  Calendar swapcalendar = UnitedStates(UnitedStates::Market::FederalReserve);
  Frequency frequency = Frequency::Annual;
  DayCounter fDayCount = Actual360();
  std::vector<std::shared_ptr<RateHelper>> sofrhelpers,liborhelpers;
  sofrhelpers.reserve(22);liborhelpers.reserve(22);
  std::shared_ptr<Sofr> sofr(new Sofr());
  std::shared_ptr<IborIndex> usdlibor = std::make_shared<USDLibor>(Period(3, Months));

  for (auto& period_str : periods){

    Period period = PeriodParser::parse(period_str);

    std::shared_ptr<SimpleQuote> s = std::make_shared<SimpleQuote>(0.05 / 100.0);
    auto start = std::chrono::steady_clock::now(); //START
    std::shared_ptr<SwapRateHelper> helper = std::make_shared<SwapRateHelper>(Handle<Quote>(s), period, swapcalendar,
                                                                              frequency, BusinessDayConvention::ModifiedFollowing,
                                                                              fDayCount, sofr);
    auto end = std::chrono::steady_clock::now(); //STOP
    std::cout << period << " sofrhelper took : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    sofrhelpers.emplace_back(helper);
  }
  REQUIRE(sofrhelpers.size()==22);

  std::cout << "====================================" << std::endl;

  for (auto& period_str : periods){

    Period period = PeriodParser::parse(period_str);

    std::shared_ptr<SimpleQuote> s = std::make_shared<SimpleQuote>(0.05 / 100.0);
    auto start = std::chrono::steady_clock::now(); //START
    std::shared_ptr<SwapRateHelper> helper = std::make_shared<SwapRateHelper>(Handle<Quote>(s), period, swapcalendar,
                                                                              frequency, BusinessDayConvention::ModifiedFollowing,
                                                                              fDayCount, usdlibor);
    auto end = std::chrono::steady_clock::now(); //STOP
    std::cout << period << " liborhelper took : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    liborhelpers.emplace_back(helper);
  }
  REQUIRE(liborhelpers.size()==22);

}