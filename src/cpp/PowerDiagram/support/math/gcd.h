#pragma once

#include "../metil_namespace.h"

BEG_METIL_NAMESPACE

template<class T,class U>
T gcd( T a, U b ) {
    if ( b == 1 )
        return 1;

    T old;
    while ( b ) {
        old = b;
        b = a % b;
        a = old;
    }
    return a;
}

END_METIL_NAMESPACE
