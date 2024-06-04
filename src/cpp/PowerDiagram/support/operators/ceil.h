#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

///
struct Functor_ceil {
    auto operator()( auto &&...a ) const;
};

/// unary
constexpr auto ceil( auto &&a ) {
    //DEFAULT_UNA_OPERATOR_CODE( ceil )

    if constexpr ( std::is_integral_v<DECAYED_TYPE_OF(a)> )
        return a;
    else
        return std::ceil( a );
}

/// binary
constexpr auto ceil( auto &&a, auto &&b ) {
    DEFAULT_BIN_OPERATOR_CODE( ceil )

    return ceil( ( a + b - 1 ) / b ) * b;
}

///
auto Functor_ceil::operator()( auto &&...a ) const { return ceil( FORWARD( a )... ); }

END_METIL_NAMESPACE
