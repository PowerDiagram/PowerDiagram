#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

/// norm
auto norm_2_p2( auto &&a ) {
    auto res = a[ 0 ] * a[ 0 ];
    for( std::size_t i = 1; i < std::size_t( a.size() ); ++i )
        res += a[ i ] * a[ i ];
    return res;
}

auto norm_2( auto &&a ) {
    using namespace std;
    return sqrt( norm_2_p2( FORWARD( a ) ) );
}


END_METIL_NAMESPACE
