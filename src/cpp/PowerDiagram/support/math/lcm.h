#pragma once

#include "gcd.h"

template<class T,class U>
T lcm( T a, U b ) {
    return a * b / gcd( a, b );
}
