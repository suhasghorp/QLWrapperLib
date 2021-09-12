//
// Created by suhasghorp on 8/16/2021.
//
#include "QLCurveBuilder.h"
#include "QLConverter.h"
#include <conventions.h>
#include <ql/termstructures/yield/piecewiseyieldcurve.hpp>
#include <ql/termstructures/yield/ratehelpers.hpp>
#include <ql/termstructures/yield/oisratehelper.hpp>
#include <ql/indexes/ibor/sofr.hpp>

using json = nlohmann::json;
using namespace QuantLib;

std::vector<Time> linspace(double start, double end, double num);

std::shared_ptr<YieldTermStructure> QLCurveBuilder::buildDiscountCurve(const std::string& valdate_str, const std::string& currency,bool enableExtrapolation,
                                                                       const json& rates){
  Calendar calendar;
  if (currency == "USD")
    calendar = UnitedStates();
  else if (currency == "EUR")
    calendar = TARGET();
  std::vector<std::shared_ptr<RateHelper>> helpers;
  helpers.reserve(100);
  const auto valdate = DateParser::parseFormatted(valdate_str, "%Y-%m-%d");
  std::shared_ptr<Sofr> sofr(new Sofr());

  const Calendar swapcalendar = UnitedStates(UnitedStates::Market::FederalReserve);
  //const Frequency frequency = Frequency::Annual;
  const DayCounter fDayCount = Actual360();
  std::stringstream ss;
  std::cout << "rates items : " << rates.size() << std::endl;
  for (auto& el : rates.items()) {

    const auto item = json::parse(el.value().dump());
    const auto ticker_str = item[0]["ticker"].get<std::string>();
    QL_REQUIRE(ticker_str.find("DEPOSIT") != std::string::npos ||
                   ticker_str.find("FUTURE") != std::string::npos ||
                   ticker_str.find("SWAP") != std::string::npos, "ticker must contain either DEPOSIT,FUTURE or SWAP");
    const auto rate = item[1]["rate"].get<double>();
    std::vector<std::string> ticker;
    boost::split(ticker, ticker_str, [](char c){return c == '-';});



    if (ticker[1] == "DEPOSIT" && ticker[2] == "1D") {
        //skip the 1D rate as explained here https://www.blog.deriscope.com/index.php/en/yield-curve-excel-quantlib-sofr
      /*std::shared_ptr<SimpleQuote> d = std::make_shared<SimpleQuote>(rate / 100.0);
        discountQuotes.emplace_back(std::pair<std::string, std::shared_ptr<SimpleQuote>>("1D", d));
        helpers.emplace_back(std::make_shared<DepositRateHelper>(Handle<Quote>(d), sofr));*/

    } else if (ticker[1] == "SWAP"){
      const auto period = get_period_from_ticker(ticker_str);
      const auto s = std::make_shared<SimpleQuote>(rate/100.0);
      ss.str("");
      ss << period;
      discountQuotes.emplace_back(std::pair<std::string,std::shared_ptr<SimpleQuote>>(ss.str(), s));
      std::shared_ptr<OISRateHelper> helper = std::make_shared<OISRateHelper>(2, period, Handle<Quote>(s), sofr, Handle<YieldTermStructure>(), true, 2, BusinessDayConvention::Following,
                                                                              Frequency::Annual,swapcalendar,Period(0*Days),0.0, Pillar::LastRelevantDate,Date(),
                                                                              RateAveraging::Compound);
      helpers.emplace_back(helper);

    }

  }

  helpers.shrink_to_fit();
  std::cout << "size of helpers " << helpers.size() << std::endl;
  const auto settlement = calendar.advance(valdate,2,Days);
  std::shared_ptr<YieldTermStructure> discountCurve(new PiecewiseYieldCurve<Discount,MonotonicLogCubic>(settlement, helpers, Actual365Fixed()));
  discountCurve->enableExtrapolation(enableExtrapolation);
  return discountCurve;
}


std::shared_ptr<YieldTermStructure> QLCurveBuilder::buildProjectionCurve(const std::string& valdate_str, const std::string& currency, bool enableExtrapolation,
                                                                         const json& rates,const json& conventions,
                                                                         std::optional<std::shared_ptr<YieldTermStructure>> discountCurve) {
  Calendar calendar;
  if (currency == "USD")
    calendar = UnitedStates();
  else if (currency == "EUR")
    calendar = TARGET();
  std::vector<std::shared_ptr<RateHelper>> helpers;
  const auto valdate = DateParser::parseFormatted(valdate_str, "%Y-%m-%d");
  const auto settlementDays = conventions["CONFIGURATIONS"]["SETTLEMENTLAG"].get<int>();

  const Date settlement = calendar.advance(valdate,settlementDays,Days);

  const DepositConventions depConventions(conventions["DEPOSIT"]["FIXINGDAYS"],
                                          to_calendar(conventions["DEPOSIT"]["CALENDAR"]),
                                          to_bus_day_convention(conventions["DEPOSIT"]["BUSINESSDAYCONVENTION"]),
                                    conventions["DEPOSIT"]["ENDOFMONTH"],
                                          to_day_counter(conventions["DEPOSIT"]["DAYCOUNTER"]));
  const FutureConventions futConventions(conventions["FUTURE"]["LENGTHINMONTHS"],
                                         to_calendar(conventions["FUTURE"]["CALENDAR"]),
                                         to_bus_day_convention(conventions["FUTURE"]["BUSINESSDAYCONVENTION"]),
                                   conventions["FUTURE"]["ENDOFMONTH"],
                                         to_day_counter(conventions["FUTURE"]["DAYCOUNTER"]));
  const SwapConventions swapConventions(to_calendar(conventions["SWAP"]["FIXEDCALENDAR"]),
                                        to_frequency(conventions["SWAP"]["FIXEDFREQUENCY"]),
                                        to_bus_day_convention(conventions["SWAP"]["FIXEDCONVENTION"]),
                                        to_day_counter(conventions["SWAP"]["FIXEDDAYCOUNTER"]),
                                  conventions["SWAP"]["FLOATINDEX"].get<std::string>());
  for (auto& el : rates.items()) {
    const auto item = json::parse(el.value().dump());
    auto ticker = item[0]["ticker"].get<std::string>();
    auto rate = item[1]["rate"].get<double>();

    QL_REQUIRE(ticker.find("DEPOSIT") != std::string::npos ||
        ticker.find("FUTURE") != std::string::npos ||
        ticker.find("SWAP") != std::string::npos, "ticker must contain either DEPOSIT,FUTURE or SWAP");

    if (ticker.find("DEPOSIT") != std::string::npos){
      if (ticker.find("1D") != std::string::npos){
        std::shared_ptr<IborIndex> usd1d(new DailyTenorUSDLibor(1));
        std::shared_ptr<SimpleQuote> d = std::make_shared<SimpleQuote>(rate/100.0);
        projectionQuotes.emplace_back(std::pair<std::string,std::shared_ptr<SimpleQuote>>("1D", d));
        helpers.push_back(std::make_shared<DepositRateHelper>(Handle<Quote>(d),usd1d));
      } else if (ticker.find("2D") != std::string::npos){
        std::shared_ptr<IborIndex> usd2d(new DailyTenorUSDLibor(2));
        std::shared_ptr<SimpleQuote> d = std::make_shared<SimpleQuote>(rate/100.0);
        projectionQuotes.emplace_back(std::pair<std::string,std::shared_ptr<SimpleQuote>>("2D", d));
        helpers.push_back(std::make_shared<DepositRateHelper>(Handle<Quote>(d),usd2d));
      } else {
        Period period = get_period_from_ticker(ticker);
        std::shared_ptr<IborIndex> usd3m(new USDLibor(period));
        std::shared_ptr<SimpleQuote> d = std::make_shared<SimpleQuote>(rate/100.0);
        std::stringstream ss;
        ss << period;
        projectionQuotes.emplace_back(std::pair<std::string,std::shared_ptr<SimpleQuote>>(ss.str(), d));
        helpers.push_back(std::make_shared<DepositRateHelper>(Handle<Quote>(d),usd3m));
      }
    } else if (ticker.find("FUTURE") != std::string::npos){
      Date startDate = get_future_start_date(ticker);
      std::shared_ptr<IborIndex> usd3m(new USDLibor(Period(3,Months)));
      std::shared_ptr<SimpleQuote> f = std::make_shared<SimpleQuote>(rate);
      helpers.push_back(std::make_shared<FuturesRateHelper>(Handle<Quote>(f), startDate, usd3m));
    } else if (ticker.find("SWAP") != std::string::npos){
      Period period = get_period_from_ticker(ticker);
      std::shared_ptr<SimpleQuote> s = std::make_shared<SimpleQuote>(rate/100.0);
      std::stringstream ss;
      ss << period;
      projectionQuotes.emplace_back(std::pair<std::string,std::shared_ptr<SimpleQuote>>(ss.str(), s));
      if (discountCurve.has_value()){
        RelinkableHandle<YieldTermStructure> discountingTermStructure;
        discountingTermStructure.linkTo(discountCurve.value());
        helpers.push_back(std::make_shared<SwapRateHelper>(Handle<Quote>(s), period, swapConventions.fixedCalendar,
                                                           swapConventions.fixedFrequency, swapConventions.fixedConvention,
                                                           swapConventions.fixedDayCounter,
                                                           to_ibor_index(swapConventions.floatIndex),
                                                           Handle<Quote>(), 0 * Days,
                                                           discountingTermStructure));
      } else {
        helpers.push_back(std::make_shared<SwapRateHelper>(Handle<Quote>(s), period, swapConventions.fixedCalendar,
                                                           swapConventions.fixedFrequency, swapConventions.fixedConvention,
                                                           swapConventions.fixedDayCounter,
                                                           to_ibor_index(swapConventions.floatIndex)));
      }
    }
  }

  DayCounter dayCounter = to_day_counter(conventions["CONFIGURATIONS"]["DAYCOUNTER"]);

  std::shared_ptr<YieldTermStructure> yieldCurve(new PiecewiseYieldCurve<ZeroYield,Cubic>(settlement, helpers, dayCounter));
  yieldCurve->enableExtrapolation(enableExtrapolation);
  std::vector<Time> tvec = linspace(0.0, 30.0, 61);
  std::stringstream ss;
  ss << "years" << ", " << "discount_factor" << "\n";
  for (Time& t : tvec){
    double df = yieldCurve->discount(t);
    ss << t << " , " << df << "\n";
  }

  return yieldCurve;
}

std::vector<Time> linspace(double start, double end, double num) {
  std::vector<Time> linspaced;
  if (0 != num)  {
    if (1 == num)    {
      linspaced.push_back(static_cast<Time>(start));
    } else {
      double delta = (end - start) / (num - 1);
      for (auto i = 0; i < (num - 1); ++i) {
        linspaced.push_back(static_cast<Time>(start + delta * i));
      }
      // ensure that start and end are exactly the same as the input
      linspaced.push_back(static_cast<Time>(end));
    }
  }
  return linspaced;
}



