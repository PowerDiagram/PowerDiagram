#pragma once

#include "default_operators.h"
// #include "../IsAlwaysZero.h"

BEG_METIL_NAMESPACE

struct Functor_self_div {
    static auto type_name() { return "METIL_NAMESPACE::Functor_self_div"; }
    auto op( auto &&a, auto &&b ) const { return FORWARD( a ) / FORWARD( b ); }
    auto operator()( auto &a, auto &&b ) const { return self_div( a, FORWARD( b ) ); }
};

constexpr auto self_div( auto &a, auto &&b ) {
    // default behavior
    DEFAULT_BIN_SELF_OPERATOR_CODE_SIGN( self_div, /=, / )

    STATIC_ASSERT_WITH_RETURN_IN_IF_CONSTEXPR( PrimitiveCtInt<0>(), 0, "found no way to call self div" );
}

END_METIL_NAMESPACE
