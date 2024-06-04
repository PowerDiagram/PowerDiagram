#pragma once

#include <cstdint>

template<class T,class U>
T ceil( T a, U m ) {
    if ( not m )
        return a;
    return ( a + m - 1 ) / m * m;
}

template<class T,class U>
T *ceil( T *a, U m ) {
    if ( not m )
        return a;
    return reinterpret_cast<T *>( ( reinterpret_cast<const std::size_t &>( a ) + m - 1 ) / m * m );
}


template<class T,class U>
T &self_ceil( T &a, U m ) {
    a = ceil( a, m );
    return a;
}
