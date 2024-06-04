#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

struct Functor_inf {
    auto operator()( auto &&a, auto &&b ) const { return FORWARD( a ) < FORWARD( b ); }
};

constexpr auto inf( auto &&a, auto &&b ) {
    DEFAULT_BIN_OPERATOR_CODE_SIGN( inf, < )

    STATIC_ASSERT_IN_IF_CONSTEXPR( false, "found not way to call inf" );
}

END_METIL_NAMESPACE
