#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

struct Functor_equ {
    auto operator()( auto &&a, auto &&b ) const { return FORWARD( a ) == FORWARD( b ); }
};

constexpr auto equ( auto &&a, auto &&b ) {
    DEFAULT_BIN_OPERATOR_CODE_SIGN( equ, == )

    STATIC_ASSERT_IN_IF_CONSTEXPR( false, "found not way to call equ" );
}

END_METIL_NAMESPACE
