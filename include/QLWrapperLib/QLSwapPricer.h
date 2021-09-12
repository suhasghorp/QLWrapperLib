//
// Created by suhasghorp on 8/21/2021.
//

#ifndef QLWRAPPERLIB_SRC_QLSWAPPRICER_H_
#define QLWRAPPERLIB_SRC_QLSWAPPRICER_H_


#include <string>
#include <vector>
#include <boost/compute/detail/lru_cache.hpp>

using boost_cache = boost::compute::detail::lru_cache<std::string, std::string>;

std::string priceSwap(std::string&& swapDef, boost_cache& cache);



#endif//QLWRAPPERLIB_SRC_QLSWAPPRICER_H_
