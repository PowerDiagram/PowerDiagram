#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

auto abs( auto &&a ) { DEFAULT_UNA_OPERATOR_CODE( abs ) STATIC_ASSERT_IN_IF_CONSTEXPR( a, "found not way to call abs" ); }

END_METIL_NAMESPACE
