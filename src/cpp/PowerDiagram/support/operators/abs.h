#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

struct Functor_abs {
    auto operator()( auto &&a ) const { using namespace std; return abs( FORWARD( a ) ); }
};

auto abs( auto &&a ) { DEFAULT_UNA_OPERATOR_CODE( abs ) STATIC_ASSERT_IN_IF_CONSTEXPR( a, "found not way to call abs" ); }

END_METIL_NAMESPACE
