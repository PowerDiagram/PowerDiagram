#pragma once

#include "../TypeConfig.h"
#include "../Vec.h"

Vec<String> split( StringView str, const String &sep = " " );

TCT Vec<String> split( StringView str, const String &sep, const T &func ) {
    Vec<String> res;

    while ( true ) {
        auto p = str.find( sep );
        if ( p == str.npos ) {
            res << func( str );
            break;
        }

        res << func( str.substr( 0, p ) );
        str.remove_prefix( p + sep.size() );
    }

    return res;
}

