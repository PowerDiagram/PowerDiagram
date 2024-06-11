#pragma once

#include "PrimitiveCtInt.h"
#include <string_view>

BEG_METIL_NAMESPACE

/// We have our own compare because <=> may be considered as a vector operation
std::ptrdiff_t compare( const auto &a, const auto &b ) {
    // as method
    if constexpr ( requires { a.compare( b ); } ) {
        return a.compare( b );
    } else

    if constexpr ( requires { b.compare( a ); } ) {
        return - b.compare( a );
    } else

    // string like
    if constexpr ( requires { std::string_view( a ); std::string_view( b ); } ) {
        return std::string_view( a ).compare( std::string_view( b ) );
    } else

    // vector like
    if constexpr ( requires { a.size(); b.size(); a[ 0 ]; b[ 0 ]; } ) {
        if ( std::ptrdiff_t d = a.size() - b.size() )
            return d;
        for( std::size_t i = 0; i < a.size(); ++i )
            if ( auto d = compare( a[ i ], b[ i ] ) )
                return d;
        return 0;
    } else

    // // tuple like
    // if constexpr ( requires { std::tuple_size<std::decay_t<decltype(a)>>{}; std::tuple_size<std::decay_t<decltype(b)>>{}; } ) {
    //     constexpr int sa = std::tuple_size<std::decay_t<decltype(a)>>{};
    //     constexpr int sb = std::tuple_size<std::decay_t<decltype(b)>>{};
    //     if constexpr ( constexpr int d = sa - sb )
    //         return d;
    //     else
    //         return compare_tuple( a, b, PrimitiveCtInt<0>(), PrimitiveCtInt<sa>() );
    // } else

    // BEWARE: does not work for float, ...
    // return std::ptrdiff_t( a ) - std::ptrdiff_t( b );

    // by default (BEWARE: does not work for float, ...)
    {
        if ( a < b )
            return -1;
        if ( a > b )
            return +1;
        return 0;
    }
}

/// Operator that can be used for std::map, std::set, ...
struct Less {
    constexpr bool operator()( const auto &a, const auto &b ) const {
        return compare( a, b ) < 0;
    }
};

/// helper to compare sets of pairs of values
inline auto compare_chain() {
    return 0;
}

auto compare_chain( const auto &a, const auto &b, const auto &...tail ) {
    if ( auto d = compare( a, b ) )
        return d;
    return compare_chain( tail... );
}

END_METIL_NAMESPACE
