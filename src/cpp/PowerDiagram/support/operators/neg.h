#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

struct Functor_neg {
    constexpr auto operator()( auto &&a ) const { return - FORWARD( a ); }
};

constexpr auto neg( auto &&a ) {
    DEFAULT_UNA_OPERATOR_CODE_SIGN( neg, - )

    STATIC_ASSERT_IN_IF_CONSTEXPR( false, "found not way to call neg" );
}

END_METIL_NAMESPACE
