#include "split.h"

Vec<String> split( StringView str, const String &sep ) {
    Vec<String> res;

    while ( true ) {
        auto p = str.find( sep );
        if ( p == str.npos ) {
            res << String{ str };
            break;
        }

        res << String{ str.substr( 0, p ) };
        str.remove_prefix( p + sep.size() );
    }

    return res;
}

