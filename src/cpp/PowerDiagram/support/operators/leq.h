#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

struct Functor_leq {
    auto operator()( auto &&a, auto &&b ) const { return FORWARD( a ) <= FORWARD( b ); }
};

constexpr auto leq( auto &&a, auto &&b ) {
    DEFAULT_BIN_OPERATOR_CODE_SIGN( leq, <= )

    STATIC_ASSERT_IN_IF_CONSTEXPR( false, "found not way to call leq" );
}

END_METIL_NAMESPACE
