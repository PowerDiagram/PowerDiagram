#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

struct Functor_geq {
    auto operator()( auto &&a, auto &&b ) const { return FORWARD( a ) >= FORWARD( b ); }
};

constexpr auto geq( auto &&a, auto &&b ) {
    DEFAULT_BIN_OPERATOR_CODE_SIGN( geq, >= )

    STATIC_ASSERT_IN_IF_CONSTEXPR( false, "found not way to call geq" );
}

END_METIL_NAMESPACE
