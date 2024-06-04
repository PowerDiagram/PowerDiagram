#pragma once

#include "default_operators.h"
#include "../IsAlwaysZero.h"

BEG_METIL_NAMESPACE

struct Functor_div {
    auto operator()( auto &&a, auto &&b ) const { return FORWARD( a ) / FORWARD( b ); }
};

constexpr auto div( auto &&a, auto &&b ) {
    // 0 / b
    if constexpr( IsAlwaysZero<DECAYED_TYPE_OF( a )>::value ) {
        return FORWARD( a );
    } else

    DEFAULT_BIN_OPERATOR_CODE_SIGN( div, / )

    STATIC_ASSERT_IN_IF_CONSTEXPR( 0, "found no way to call div" );
}

END_METIL_NAMESPACE
