#pragma once

#include <tl/support/containers/operators/ceil.h>
#include <tl/support/containers/Vec.h>
#include <xsimd/xsimd.hpp>
#include <vector>

/**
*/
template<class T,int nb_dims>
class SimdTensor {
public:
    using               AlignedVec = std::vector<T,xsimd::default_allocator<T>>;
    using               SimdVec    = xsimd::batch<T>;
    using               Point      = Vec<T,nb_dims>;

    static constexpr PI simd_size  = SimdVec::size;

    /**/                SimdTensor ();

    SimdTensor&         operator=  ( const SimdTensor &that );

    PI                  capacity   () const { return _data.size() / nb_dims; }
    PI                  size       () const { return _size; }

    PI                  offset     ( PI n ) const { return ( n / simd_size ) * simd_size * nb_dims + ( n % simd_size ); }
    PI                  offset     ( PI n, PI d ) const { return offset( n ) + simd_size * d; }

    const T&            operator() ( PI n, PI d ) const { return _data[ offset( n, d ) ]; }
    T&                  operator() ( PI n, PI d ) { return _data[ offset( n, d ) ]; }
    Point               operator[] ( PI n ) const { Point res; for( PI d = 0; d < nb_dims; ++d ) res[ d ] = operator()( n, d ); return res; }

    const T*            data       () const { return _data.data(); }
    T*                  data       () { return _data.data(); }

    SimdTensor&         operator<< ( const Point &p );
    void                set_item   ( PI index, const Point &p );
    void                reserve    ( PI capa );
    void                resize     ( PI size );
    void                clear      ();

private:
    PI                  _size;
    AlignedVec          _data;     ///< ex for simd_size == 4: x0 x1 x2 x3 y0 y1 y2 y3 x4 x5 ...
};

#define DTP template<class T,int nb_dims>
#define UTP SimdTensor<T,nb_dims>

DTP UTP::SimdTensor() : _size( 0 ) {
    reserve( 128 );
}

DTP UTP &UTP::operator=( const SimdTensor &that ) {
    resize( that.size() );
    for( PI i = 0; i < that.size(); ++i )
        set_item( i, that[ i ] );
    return *this;
}

DTP UTP &UTP::operator<<( const Point &p ) {
    const PI o = _size++;
    reserve( _size );
    set_item( o, p );
    return *this;
}

DTP void UTP::set_item( PI index, const Point &p ) {
    for( PI d = 0; d < nb_dims; ++d )
        operator()( index, d ) = p[ d ];
}

DTP void UTP::reserve( PI capa ) {
    const PI wsize = ceil( capa, simd_size ) * nb_dims;
    if ( wsize > _data.size() )
        _data.resize( wsize );
}

DTP void UTP::resize( PI size ) {
    reserve( size );
    _size = size;
}

DTP void UTP::clear() {
    _size = 0;
}

#undef DTP
#undef UTP
