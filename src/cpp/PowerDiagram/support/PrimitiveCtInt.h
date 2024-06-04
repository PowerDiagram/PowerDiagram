#pragma once

#include "metil_namespace.h"
#include <string>

BEG_METIL_NAMESPACE

template<int i>
struct PrimitiveCtInt {
    static void          for_each_template_arg ( auto &&f ) { f( PrimitiveCtInt<i>() ); }
    static auto          template_type_name    () { return "PrimitiveCtInt"; }
    static auto          to_string             () { return std::to_string( value ); }
    static constexpr int value                 = i;
};

template<int a,int b>
constexpr auto max( PrimitiveCtInt<a>, PrimitiveCtInt<b> ) { return PrimitiveCtInt<( a >= b ? a : b )>(); }

template<int a,int b>
constexpr bool operator>=( PrimitiveCtInt<a>, PrimitiveCtInt<b> ) { return a >= b; }

END_METIL_NAMESPACE
