#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

/// scalar product
auto sp( auto &&a, auto &&b ) {
    auto res = a[ 0 ] * b[ 0 ];
    for( std::size_t i = 1; i < std::min( std::size_t( a.size() ), std::size_t( b.size() ) ); ++i )
        res += a[ i ] * b[ i ];
    return res;
}

END_METIL_NAMESPACE
