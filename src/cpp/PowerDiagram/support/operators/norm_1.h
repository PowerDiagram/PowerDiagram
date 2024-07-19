#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

/// norm
auto norm_1( auto &&a ) -> decltype( a[ 0 ] + a[ 1 ] ) {
    using std::abs;
    if ( a.size() == 0 )
        return 0;
    auto res = abs( a[ 0 ] );
    for( std::size_t i = 1; i < std::size_t( a.size() ); ++i )
        res += abs( a[ i ] );
    return res;
}

END_METIL_NAMESPACE
