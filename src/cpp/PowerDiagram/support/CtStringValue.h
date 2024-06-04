#pragma once

#include "metil_namespace.h"
#include <algorithm>
#include <string>

BEG_METIL_NAMESPACE

template<std::size_t N>
struct CtStringValue {
    constexpr   CtStringValue( const char (&str)[N] ) { std::copy_n( str, N, value ); }

    std::string to_string    () const { return { value, value + N - 1 }; }

    char        value[N];
};

END_METIL_NAMESPACE
