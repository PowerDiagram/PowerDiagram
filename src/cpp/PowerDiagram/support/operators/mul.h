#pragma once

#include "default_operators.h"
#include "../IsAlwaysZero.h"

BEG_METIL_NAMESPACE

struct Functor_mul {
    auto operator()( auto &&a, auto &&b ) const { return FORWARD( a ) * FORWARD( b ); }
};

constexpr auto mul( auto &&a, auto &&b ) {
    // 0 * b
    if constexpr( IsAlwaysZero<DECAYED_TYPE_OF( a )>::value ) {
        return FORWARD( a );
    } else

    // a * 0
    if constexpr( IsAlwaysZero<DECAYED_TYPE_OF( b )>::value ) {
        return FORWARD( b );
    } else

    DEFAULT_BIN_OPERATOR_CODE_SIGN( mul, * )

    STATIC_ASSERT_IN_IF_CONSTEXPR( 0, "found no way to call mul" );
}

/// more than 2 operands
constexpr auto mul( auto &&a, auto &&b, auto &&c, auto &&...d ) {
    return mul( mul( FORWARD( a ), FORWARD( b ) ), FORWARD( c ), FORWARD( d )... );
}

END_METIL_NAMESPACE
