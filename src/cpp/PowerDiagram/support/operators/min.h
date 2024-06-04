#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

///
struct Functor_min {
    auto operator()( auto &&...a ) const;
};

/// binary
constexpr auto min( auto &&a, auto &&b ) {
    DEFAULT_BIN_OPERATOR_CODE( min )

    return a <= b ? FORWARD( a ) : FORWARD( b );
}

/// more than 2 operands
constexpr auto min( auto &&a, auto &&b, auto &&c, auto &&...d ) {
    return min( min( FORWARD( a ), FORWARD( b ) ), FORWARD( c ), FORWARD( d )... );
}

/// seq min
auto min( auto &&a ) requires ( TensorOrder<DECAYED_TYPE_OF( a )>::value == 1 ) {
    using std::min;
    auto res = a[ 0 ];
    for( std::size_t i = 1; i < a.size(); ++i )
        res = min( res, a[ i ] );
    return res;
}

///
auto Functor_min::operator()( auto &&...a ) const { return min( FORWARD( a )... ); }

END_METIL_NAMESPACE
