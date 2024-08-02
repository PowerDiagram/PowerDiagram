#pragma once

#include "abs.h"

BEG_METIL_NAMESPACE

/// norm 1
auto norm_1( auto &&a ) -> std::decay_t<decltype( abs( a[ 0 ] ) + abs( a[ 1 ] ) )> {
    if ( a.size() == 0 )
        return 0;
    using TR = std::decay_t<decltype( abs( a[ 0 ] ) + abs( a[ 1 ] ) )>;
    TR res = abs( a[ 0 ] );
    for( std::size_t i = 1; i < std::size_t( a.size() ); ++i )
        res += abs( a[ i ] );
    return res;
}

END_METIL_NAMESPACE
