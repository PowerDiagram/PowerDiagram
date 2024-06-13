#pragma once

#include <boost/multiprecision/gmp.hpp>
// #include "common_types.h"
#include "CtType.h"

template<unsigned digits_10>
using BoostMpf = boost::multiprecision::number<boost::multiprecision::gmp_float<digits_10>>;

template<unsigned digits_10>
inline auto type_name( CtType<BoostMpf<digits_10>> ) {
    return "Mpf<digits_10>>";
}
