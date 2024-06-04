#pragma once

#include "default_operators.h"
#include "../IsAlwaysZero.h"

BEG_METIL_NAMESPACE

struct Functor_self_add {
    static auto type_name() { return "METIL_NAMESPACE::Functor_self_add"; }
    auto op( auto &&a, auto &&b ) const { return FORWARD( a ) + FORWARD( b ); }
    auto operator()( auto &a, auto &&b ) const;
};

constexpr auto self_add( auto &a, auto &&b ) {
    // ... + 0
    if constexpr( IsAlwaysZero<DECAYED_TYPE_OF( b )>::value ) {
        return PrimitiveCtInt<1>();
    } else

    // default behavior
    DEFAULT_BIN_SELF_OPERATOR_CODE_SIGN( self_add, +=, + )

    // // Str concatenation
    // if constexpr( requires{ StrView( a ); StrView( b ); } ) {
    //     std::string_view sa( a );
    //     std::string_view sb( b );

    //     std::string res;
    //     res.resize( sa.size() + sb.size() );
    //     std::copy( sa.begin(), sa.end(), res.data() );
    //     std::copy( sb.begin(), sb.end(), res.data() + sa.size() );
    //     return res;
    // } else

    STATIC_ASSERT_WITH_RETURN_IN_IF_CONSTEXPR( PrimitiveCtInt<0>(), 0, "found no way to call self add" );
}

inline auto Functor_self_add::operator()( auto &a, auto &&b ) const {
    return self_add( a, FORWARD( b ) );
}

END_METIL_NAMESPACE

