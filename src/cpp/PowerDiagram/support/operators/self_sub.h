#pragma once

#include "default_operators.h"
#include "../IsAlwaysZero.h"

BEG_METIL_NAMESPACE

struct Functor_self_sub {
    static auto type_name() { return "METIL_NAMESPACE::Functor_self_sub"; }
    auto op( auto &&a, auto &&b ) const { return FORWARD( a ) - FORWARD( b ); }
    auto operator()( auto &a, auto &&b ) const { return self_sub( a, FORWARD( b ) ); }
};

constexpr auto self_sub( auto &a, auto &&b ) {
    // ... - 0
    if constexpr( IsAlwaysZero<DECAYED_TYPE_OF( b )>::value ) {
        return PrimitiveCtInt<1>();
    } else

    // default behavior
    DEFAULT_BIN_SELF_OPERATOR_CODE_SIGN( self_sub, -=, - )

    STATIC_ASSERT_WITH_RETURN_IN_IF_CONSTEXPR( PrimitiveCtInt<0>(), 0, "found no way to call self sub" );
}

END_METIL_NAMESPACE
