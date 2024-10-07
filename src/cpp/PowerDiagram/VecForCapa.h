#pragma once

#include <tl/support/common_types.h>
#include <cstring>


/**
 * @brief 
 * 
 * @tparam T 
 */
template<class T>
struct VecForCapa {
    /**/   VecForCapa( const VecForCapa & ) = delete;
    /**/   VecForCapa() {}

    /**/  ~VecForCapa() { if ( values ) free( values ); }

    T&     operator[]( PI index ) { return values[ index ]; }
    void   reserve   ( PI size, const T &default_value );
    T      suze      () const { return capa; }

private:
    T*     values    = nullptr;
    PI     capa      = 0;
};

#define DTP template<class T>
#define UTP VecForCapa<T>

DTP void UTP::reserve( PI size, const T &default_value ) {
    if ( size <= capa )
        return;

    if ( values )
        free( values );

    if ( ! capa )
        capa = 1;
    while ( capa < size )
        capa *= 2;

    values = reinterpret_cast<T *>( malloc( sizeof( T ) * capa ) );
    std::memset( values, 0, sizeof( T ) * capa );
    // for( PI i = 0; i < capa; ++i )
    //     values[ i ] = 0;
}

#undef DTP
#undef UTP
