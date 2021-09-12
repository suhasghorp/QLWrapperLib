//
// Created by suhasghorp on 8/21/2021.
//

#include "QLConverter.h"

using namespace QuantLib;

Date to_date(const std::string& s){
  return DateParser::parseFormatted(s, "%Y-%m-%d");
}

BusinessDayConvention to_bus_day_convention(const std::string& c){
  auto upper = boost::to_upper_copy<std::string>(c);
  if (upper == "FOLLOWING") return Following;
  if (upper == "MODIFIEDFOLLOWING") return ModifiedFollowing;
  if (upper == "PRECEDING") return Preceding;
  if (upper == "MODIFIEDPRECEDING") return ModifiedPreceding;
  if (upper == "UNADJUSTED") return Unadjusted;
  QL_FAIL(upper << " could not be converted to Business Day Convention");
}

Calendar to_calendar(const std::string& c) {
  auto upper = boost::to_upper_copy<std::string>(c);
  if (upper == "TARGET") return TARGET();
  if (upper == "UNITEDSTATES") return UnitedStates(UnitedStates::FederalReserve);
  if (upper == "UNITEDKINGDOM") return UnitedKingdom();
  QL_FAIL(upper << " could not be converted to Calendar");
}

Swap::Type to_swap_type(const std::string& s){
  auto upper = boost::to_upper_copy<std::string>(s);
  if (upper == "PAYER") return Swap::Payer;
  if (upper == "RECEIVER") return Swap::Receiver;
  QL_FAIL(upper << " could not be converted to Swap Type");
}

Frequency to_frequency(const std::string& f){
  auto upper = boost::to_upper_copy<std::string>(f);
  if (upper == "DAILY") return Daily;
  if (upper == "WEEKLY") return Weekly;
  if (upper == "MONTHLY") return Monthly;
  if (upper == "QUARTERLY") return Quarterly;
  if (upper == "SEMIANNUAL") return Semiannual;
  if (upper == "ANNUAL") return Annual;
  QL_FAIL(upper << " could not be converted to Frequency");
}

DateGeneration::Rule to_date_gen_rule(const std::string& d){
  auto upper = boost::to_upper_copy<std::string>(d);
  if (upper == "BACKWARD") return DateGeneration::Backward;
  if (upper == "FORWARD") return DateGeneration::Forward;
  QL_FAIL(upper << " could not be converted to Date Generation Rule");
}

DayCounter to_day_counter(const std::string& d){
  auto upper = boost::to_upper_copy<std::string>(d);
  if (upper == "ACTUAL360") return Actual360();
  if (upper == "ACTUAL365FIXED") return Actual365Fixed();
  if (upper == "ACTUALACTUAL") return ActualActual(ActualActual::Convention::ISDA, Schedule());
  if (upper == "BUSINESS252") return Business252();
  if (upper == "ONEDAYCOUNTER") return OneDayCounter();
  if (upper == "SIMPLEDAYCOUNTER") return SimpleDayCounter();
  if (upper == "THIRTY360") return Thirty360(Thirty360::Convention::BondBasis, Date());
  QL_FAIL(upper << " could not be converted to Day Counter");
}

Period get_period_from_string(const std::string& period){
  return PeriodParser::parse(period);
}

Period get_period_from_ticker(const std::string& ticker){
  std::vector<std::string> results;
  boost::split(results, ticker, [](char c){return c == '-';});
  QL_REQUIRE(results.size() == 3, "ticker format is not correct");
  return get_period_from_string(results[2]);
}

std::shared_ptr<IborIndex> to_ibor_index(const std::string& s){
  std::vector<std::string> results;
  boost::split(results, s, [](char c){return c == '.';});
  QL_REQUIRE(results.size() == 2, "Swap Floating Index is not in the correct format");
  auto upper = boost::to_upper_copy<std::string>(results[0]);
  if(upper == "USD") return std::make_shared<USDLibor>(get_period_from_string(results[1]));
  if(upper == "EUR") return std::make_shared<Euribor>(get_period_from_string(results[1]));
}

Date get_future_start_date(std::string ticker){
  std::vector<std::string> results;
  boost::split(results, ticker, [](char c){return c == '-';});
  QL_REQUIRE(results.size() == 3, "ticker format is not correct");
  std::string monthYear = results[2];
  results.clear();
  boost::split(results, monthYear, [](char c){return c == ' ';});
  QL_REQUIRE(results.size() == 2, "ticker format is not correct");
  std::unordered_map<std::string, Month> dict {{"Sep", Month::September}, {"Dec", Month::December}, {"Mar", Month::March}, {"Jun", Month::June}};
  int year = stoi(results[1]);
  Date d = IMM::nextDate(Date(1, dict.at(results[0]),year));
  return d;

}
