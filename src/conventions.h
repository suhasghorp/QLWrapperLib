//
// Created by suhasghorp on 8/25/2021.
//

#ifndef QLWRAPPERLIB_SRC_CONVENTIONS_H_
#define QLWRAPPERLIB_SRC_CONVENTIONS_H_

struct DepositConventions {
  int fixingDays;
  Calendar calendar;
  BusinessDayConvention businessDayConvention;
  bool endOfMonth;
  DayCounter dayCounter;

 public:
  explicit DepositConventions(int fixingDays,
                              Calendar calendar,
                              BusinessDayConvention businessDayConvention,
                              bool endOfMonth,
                              DayCounter dayCounter):
      fixingDays(fixingDays),calendar(calendar),businessDayConvention(businessDayConvention),endOfMonth(endOfMonth),dayCounter(dayCounter) {}
};

struct FutureConventions {
  int lengthInMonths;
  Calendar calendar;
  BusinessDayConvention businessDayConvention;
  bool endOfMonth;
  DayCounter dayCounter;

 public:
  explicit FutureConventions(int lengthInMonths,
                             Calendar calendar,
                             BusinessDayConvention businessDayConvention,
                             bool endOfMonth,
                             DayCounter dayCounter):
      lengthInMonths(lengthInMonths),calendar(calendar),businessDayConvention(businessDayConvention),endOfMonth(endOfMonth),dayCounter(dayCounter) {}
};

struct SwapConventions {
  Calendar fixedCalendar;
  Frequency fixedFrequency;
  BusinessDayConvention fixedConvention;
  DayCounter fixedDayCounter;
  std::string floatIndex;

 public:
  explicit SwapConventions(Calendar fixedCalendar,
                           Frequency fixedFrequency,
                           BusinessDayConvention fixedConvention,
                           DayCounter fixedDayCounter,
                           std::string floatIndex):
      fixedCalendar(fixedCalendar), fixedFrequency(fixedFrequency),fixedConvention(fixedConvention),fixedDayCounter(fixedDayCounter), floatIndex(floatIndex) {}
};

#endif//QLWRAPPERLIB_SRC_CONVENTIONS_H_
