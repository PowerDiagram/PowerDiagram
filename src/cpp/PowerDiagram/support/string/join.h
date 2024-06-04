#pragma once

#include "../common_includes.h"
#include "to_string.h"

BEG_METIL_NAMESPACE

Str join( const auto &vec, Str sep = ", " ) {
    Str res;
    for( const auto &v : vec ) {
        if ( res.size() )
            res += sep;
        res += to_string( v );
    }
    return res;
}

inline Str join_binary( const auto &vec, Str func = "std::max" ) {
    ASSERT( vec.size() );

    Str res = vec[ 0 ];
    for( PI i = 1; i < vec.size(); ++i )
        res = func + "( " + res + ", " + vec[ i ] + " )";
    return res;
}

END_METIL_NAMESPACE
