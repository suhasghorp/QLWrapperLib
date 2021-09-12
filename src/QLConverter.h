//
// Created by suhasghorp on 8/16/2021.
//

#ifndef QLWRAPPERLIB_SRC_QLCONVERTER_H_
#define QLWRAPPERLIB_SRC_QLCONVERTER_H_

#include <ql/utilities/dataparsers.hpp>
#include <ql/time/businessdayconvention.hpp>
#include <ql/time/calendars/target.hpp>
#include <ql/time/imm.hpp>
#include <ql/time/calendars/unitedstates.hpp>
#include <ql/time/calendars/unitedkingdom.hpp>
#include <ql/instruments/vanillaswap.hpp>
#include <ql/time/daycounters/actualactual.hpp>
#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/daycounters/actual365fixed.hpp>
#include <ql/time/daycounters/business252.hpp>
#include <ql/time/daycounters/one.hpp>
#include <ql/time/daycounters/simpledaycounter.hpp>
#include <ql/time/daycounters/thirty360.hpp>
#include <boost/algorithm/string.hpp>
#include <ql/indexes/ibor/usdlibor.hpp>
#include <ql/indexes/ibor/euribor.hpp>


using namespace QuantLib;

Date to_date(const std::string& s);

BusinessDayConvention to_bus_day_convention(const std::string& c);

Calendar to_calendar(const std::string& c);

Swap::Type to_swap_type(const std::string& s);

Frequency to_frequency(const std::string& f);

DateGeneration::Rule to_date_gen_rule(const std::string& d);

DayCounter to_day_counter(const std::string& d);

Period get_period_from_string(const std::string& period);

Period get_period_from_ticker(const std::string& ticker);

std::shared_ptr<IborIndex> to_ibor_index(const std::string& s);

Date get_future_start_date(std::string ticker);


#endif//QLWRAPPERLIB_SRC_QLCONVERTER_H_
