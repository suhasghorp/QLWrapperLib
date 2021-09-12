//
// Created by suhasghorp on 8/16/2021.
//

#ifndef QLWRAPPERLIB_SRC_QLCURVEBUILDER_H_
#define QLWRAPPERLIB_SRC_QLCURVEBUILDER_H_

#include "QLDynamoDBHelper.h"
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/quotes/simplequote.hpp>

#include <string>

using namespace QuantLib;

struct QLCurveBuilder {
  std::vector<std::pair<std::string,std::shared_ptr<SimpleQuote>>> projectionQuotes;
  std::vector<std::pair<std::string,std::shared_ptr<SimpleQuote>>> discountQuotes;
  std::shared_ptr<YieldTermStructure> buildProjectionCurve(const std::string& valdate_str, const std::string& currency,bool enableExtrapolation,
                                                           const json& rates,const json& conventions,
                                                           std::optional<std::shared_ptr<YieldTermStructure>> discountCurve) ;
  std::shared_ptr<YieldTermStructure> buildDiscountCurve(const std::string& valdate_str, const std::string& currency,bool enableExtrapolation,
                                                         const json& rates) ;
};





#endif//QLWRAPPERLIB_SRC_QLCURVEBUILDER_H_
