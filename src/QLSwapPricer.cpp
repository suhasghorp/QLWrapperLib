//
// Created by suhasghorp on 8/21/2021.
//

#include "QLConverter.h"
#include "QLCurveBuilder.h"
#include <QLWrapperLib/QLSwapPricer.h>
#include <ql/pricingengines/swap/discountingswapengine.hpp>
#include <ql/termstructures/yield/zerospreadedtermstructure.hpp>
#include <ql/experimental/termstructures/multicurvesensitivities.hpp>


using namespace QuantLib;
using boost_cache = boost::compute::detail::lru_cache<std::string, std::string>;

Date closest(const std::vector<Date>& vec, Date d) {
  auto const it = std::lower_bound(vec.begin(), vec.end(), d);
  if (it == vec.begin()) { return *it; }
  else return *(it - 1);
}

std::string to_string_with_precision(const double a_value, const int n = 6)
{
  std::ostringstream out;
  out.precision(n);
  out << std::fixed << a_value;
  return out.str();
}

json getRiskFreeRates(boost_cache& cache, const std::string& valdate_str, const std::string& currency){
  json discRates;
  const auto cacheDiscKey = currency == "USD" ?  valdate_str + "|SOFR" : valdate_str + "|ESTR";
  boost::optional<std::string> maybe = cache.get(cacheDiscKey);
  if (maybe) {//sofr rates were found in the cache
    std::cout << "discount rates were found in the cache" << std::endl;
    discRates = json::parse(maybe.get());
  } else {
    std::cout << "discount rates were not in the cache" << std::endl;
    discRates = dynamo::getRiskFreeRates(valdate_str, currency);
    cache.insert(cacheDiscKey, discRates.dump());
  }
  return discRates;
}

json getLiborRates(boost_cache& cache, const std::string& valdate_str, const std::string& currency){
  json forecastRates;
  const auto cacheForecastKey = currency == "USD" ?  valdate_str + "|USD-LIBOR-3M" : valdate_str + "|EUR-LIBOR-6M";
  boost::optional<std::string> maybe = cache.get(cacheForecastKey);
  if (maybe) {//libor rates were found in the cache
    std::cout << "forecast rates were found in the cache" << std::endl;
    forecastRates = json::parse(maybe.get());
  } else {
    std::cout << "forecast rates were not in the cache" << std::endl;
    forecastRates = dynamo::getLiborRates(valdate_str, currency);
    cache.insert(cacheForecastKey, forecastRates.dump());
  }
  return forecastRates;
}

json getConventions(boost_cache& cache, const std::string& currency){
  json conventions;
  const auto convKey = currency == "USD" ?  "USD-CONVENTIONS" : "EUR-CONVENTIONS";
  boost::optional<std::string> maybe = cache.get(convKey);
  if (maybe) {//conventions were found in the cache
    std::cout << "conventions were found in the cache" << std::endl;
    conventions = json::parse(maybe.get());
  } else {
    std::cout << "conventions were not in the cache" << std::endl;
    conventions = dynamo::getConventions(currency);
    cache.insert(convKey, conventions.dump());
  }
  return conventions;
}

double getFixing(boost_cache& cache, const std::string& key, const std::string& date_str){
  double fixing;
  const std::string cacheKey = key + "|" + date_str;
  boost::optional<std::string>  maybeFixing = cache.get(cacheKey);
  if (maybeFixing){
    std::cout << "fixing was found in the cache" << std::endl;
    fixing = std::stod(maybeFixing.get().c_str());
  } else {
    std::cout << "fixing was found not in the cache" << std::endl;
    fixing = fixing = dynamo::getFixing(key, date_str);
    cache.insert(key, to_string_with_precision(fixing,7));
  }
  return fixing;
}



std::string priceSwap(std::string&& swapDef, boost_cache& cache){

  try {

    /*json temp = R"({"valDate": "2021-08-16", "currency":"USD", "swapType": "PAYER", "nominal": 48000000.0, "startDate": "2018-03-14", "maturityDate": "2028-03-14", "fixedLegFrequency": "SEMIANNUAL",
"fixedLegCalendar": "UNITEDSTATES", "fixedLegConvention": "MODIFIEDFOLLOWING", "fixedLegDateGenerationRule": "BACKWARD", "fixedLegRate": 0.02,
"fixedLegDayCount": "ACTUAL360", "fixedLegEndOfMonth": false, "floatingLegFrequency": "QUARTERLY", "floatingLegCalendar": "UNITEDSTATES",
"floatingLegConvention": "MODIFIEDFOLLOWING", "floatingLegDateGenerationRule": "BACKWARD", "floatingLegSpread": 0.0007, "floatingLegDayCount": "ACTUAL360", "floatingLegEndOfMonth": false})"_json;*/

    json temp = json::parse(swapDef);


    auto valdate_str = temp["valDate"].get<std::string>();
    Date valdate = DateParser::parseFormatted(valdate_str, "%Y-%m-%d");
    Settings::instance().evaluationDate() = valdate;
    auto currency = temp["currency"].get<std::string>();
    const auto discRates = getRiskFreeRates(cache, valdate_str,currency);
    const auto forecastRates = getLiborRates(cache,valdate_str,currency);
    const auto conventions = getConventions(cache,currency);

    QLCurveBuilder builder;

    RelinkableHandle<YieldTermStructure> discountingTermStructure;
    std::shared_ptr<YieldTermStructure> discountCurve = builder.buildDiscountCurve(valdate_str, currency,true,discRates);
    discountingTermStructure.linkTo(discountCurve);

    std::optional<std::shared_ptr<YieldTermStructure>> opt(discountCurve);

    RelinkableHandle<YieldTermStructure> forecastingTermStructure;
    std::shared_ptr<YieldTermStructure> yieldCurve = builder.buildProjectionCurve(valdate_str, currency, true, forecastRates, conventions, opt);
    forecastingTermStructure.linkTo(yieldCurve);


    Period floatingLegPeriod = Period(to_frequency((temp["floatingLegFrequency"].get<std::string>())));
    std::shared_ptr<IborIndex> index(new USDLibor(floatingLegPeriod, forecastingTermStructure));

    Date startDate = to_date(temp["startDate"].get<std::string>());
    Date maturityDate = to_date(temp["maturityDate"].get<std::string>());
    Period p = Period(to_frequency((temp["fixedLegFrequency"].get<std::string>())));
    Calendar cal = to_calendar(temp["fixedLegCalendar"].get<std::string>());
    BusinessDayConvention conv = to_bus_day_convention(temp["fixedLegConvention"].get<std::string>());
    DateGeneration::Rule dateRule = to_date_gen_rule(temp["fixedLegDateGenerationRule"].get<std::string>());

    Schedule fixedSchedule = MakeSchedule().from(startDate).to(maturityDate).withTenor(p).withCalendar(cal).withConvention(conv).withTerminationDateConvention(conv).withRule(dateRule);

    p = Period(to_frequency((temp["floatingLegFrequency"].get<std::string>())));
    cal = to_calendar(temp["floatingLegCalendar"].get<std::string>());
    conv = to_bus_day_convention(temp["floatingLegConvention"].get<std::string>());
    dateRule = to_date_gen_rule(temp["floatingLegDateGenerationRule"].get<std::string>());

    Schedule floatSchedule = MakeSchedule().from(startDate).to(maturityDate).withTenor(p).withCalendar(cal).withConvention(conv).withTerminationDateConvention(conv).withRule(dateRule);

    Swap::Type type = to_swap_type(temp["swapType"].get<std::string>());
    auto nominal = temp["nominal"].get<double>();
    auto fixedRate = temp["fixedLegRate"].get<double>();
    DayCounter fixedDayCount = to_day_counter(temp["fixedLegDayCount"].get<std::string>());
    auto floatingSpread = temp["floatingLegSpread"].get<double>();

    std::shared_ptr<VanillaSwap> swap(
        new VanillaSwap(type, nominal,
                        fixedSchedule, fixedRate, fixedDayCount,
                        floatSchedule, index, floatingSpread,
                        index->dayCounter()));

    Date prevFixingDate = closest(floatSchedule.dates(), valdate);
    prevFixingDate = index->fixingCalendar().advance(prevFixingDate, -2, Days);
    std::stringstream ss;
    ss << std::fixed;
    ss << index->tenor();
    std::string key;
    if (index->familyName() == "USDLibor" && ss.str() == "3M") {
      key = "usd-libor-3m";
    }
    ss.str("");
    ss << QuantLib::io::iso_date(prevFixingDate);
    double fixing = getFixing(cache, key, ss.str());
    std::cout << "previous fixing " << fixing << std::endl;
    index->addFixing(prevFixingDate, fixing);

    swap->setPricingEngine(std::shared_ptr<PricingEngine>(new DiscountingSwapEngine(discountingTermStructure)));
    std::cout << "swap pricing engine set" << std::endl;
    ss.str("");
    double npv = swap->NPV();
    double fairRate = swap->fairRate();

    std::cout << "swap NPV: " << npv << std::endl;
    ss << "{ \"npv\" : " << npv << "}";
    std::cout << ss.str() << std::endl;

    std::cout << "swap fair rate: " << npv << std::endl;
    ss << "{ \"Fair Rate\" : " << fairRate << "}";
    std::cout << ss.str() << std::endl;

    const double shift = 0.0001;

    RelinkableHandle<YieldTermStructure> tempProjHandle, tempDiscHandle;
    tempProjHandle.linkTo(yieldCurve);
    tempDiscHandle.linkTo(discountCurve);
    std::shared_ptr<YieldTermStructure> shiftedProjCurve = std::make_shared<ZeroSpreadedTermStructure>(tempProjHandle, Handle<Quote>(std::make_shared<SimpleQuote>(shift)));
    std::shared_ptr<YieldTermStructure> shiftedDiscCurve = std::make_shared<ZeroSpreadedTermStructure>(tempDiscHandle, Handle<Quote>(std::make_shared<SimpleQuote>(shift)));
    forecastingTermStructure.linkTo(shiftedProjCurve);
    discountingTermStructure.linkTo(shiftedDiscCurve);
    double npvUp = swap->NPV();

    shiftedProjCurve = std::make_shared<ZeroSpreadedTermStructure>(tempProjHandle, Handle<Quote>(std::make_shared<SimpleQuote>(-1.0 * shift)));
    shiftedDiscCurve = std::make_shared<ZeroSpreadedTermStructure>(tempDiscHandle, Handle<Quote>(std::make_shared<SimpleQuote>(-1.0 * shift)));
    forecastingTermStructure.linkTo(shiftedProjCurve);
    discountingTermStructure.linkTo(shiftedDiscCurve);
    double npvDown = swap->NPV();

    double dv01 = (npvDown - npvUp)/2.0;

    std::cout << "swap DV01: " << dv01 << std::endl;
    ss << "{ \"DV01\" : " << dv01 << "}";
    std::cout << ss.str() << std::endl;

    forecastingTermStructure.linkTo(yieldCurve);
    discountingTermStructure.linkTo(discountCurve);

    ss << "{ \"key rate durations\" : " ;

    for (auto const& pair : builder.projectionQuotes){
      double value = pair.second->value();
      pair.second->setValue(value + shift);
      double keyRisk = npv - swap->NPV();
      pair.second->setValue(value);
      ss << "{ \"" << pair.first << "\" : " << keyRisk << "}";
    }
    ss << "}";
    std::cout << ss.str() << std::endl;

    return ss.str();
  } catch (...) {
    throw;
  }


}
