#pragma once

#include "PointTree.h"

/**
*/
template<class Scalar,int nb_dims>
class PointTreeWithValues : public PointTree<Scalar,nb_dims> {
public:
    using                Point              = PointTree<Scalar,nb_dims>::Point;

    /**/                 PointTreeWithValues( Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree<Scalar,nb_dims> *parent, PI num_in_parent ) : PointTree<Scalar,nb_dims>( parent, num_in_parent ), indices( indices ), weights( weights ), points( points ) {}

    virtual void         for_each_point     ( const std::function<void( const Point &p0, const Scalar &w0, const PI i0 )> &f, Vec<PI32> &buffer, const Point &center );
    virtual void         for_each_point     ( const std::function<void( const Point &p0, const Scalar &w0, const PI i0 )> &f ) { for( PI n0 = 0, nc = points.size(); n0 < nc; ++n0 ) f( points[ n0 ], weights[ n0 ], indices[ n0 ] ); }
    virtual PI           nb_seed_points     () const { return indices.size(); }

    Span<PI>             indices;
    Span<Scalar>         weights;
    Span<Point>          points;
};

#define DTP template<class Scalar,int nb_dims>
#define UTP PointTreeWithValues<Scalar,nb_dims>

DTP void UTP::for_each_point( const std::function<void( const Point &p0, const Scalar &w0, const PI i0 )> &f, Vec<PI32> &buffer, const Point &center ) {
    buffer.clear();
    for( PI ni = 0, nc = points.size(); ni < nc; ++ni )
        buffer << ni;

    std::sort( buffer.begin(), buffer.end(), [&]( const auto &a, const auto &b ) {
        return norm_2_p2( points[ a ] - center ) < norm_2_p2( points[ a ] - center );
    } );

    for( PI n : buffer )
        f( points[ n ], weights[ n ], indices[ n ] );
}


#undef DTP
#undef UTP
