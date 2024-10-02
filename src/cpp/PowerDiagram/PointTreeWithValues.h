#pragma once

#include "PointTree.h"

/**
*/
template<class Scalar,int nb_dims>
class PointTreeWithValues : public PointTree<Scalar,nb_dims> {
public:
    using                Point              = PointTree<Scalar,nb_dims>::Point;

    /**/                 PointTreeWithValues( Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree<Scalar,nb_dims> *parent, PI num_in_parent ) : PointTree<Scalar,nb_dims>( parent, num_in_parent ), indices( indices ), weights( weights ), points( points ) {}

    virtual void         for_each_point     ( const std::function<void( const Point &p0, const Scalar &w0, const PI i0 )> &f ) { for( PI n0 = 0, nc = points.size(); n0 < nc; ++n0 ) f( points[ n0 ], weights[ n0 ], indices[ n0 ] ); }
    virtual PI           nb_seed_points     () const { return indices.size(); }
    virtual void         get_otps           ( Vec<std::tuple<Point,Scalar,PI>> &otps, const Point &p0, PI i0 ) override;

    Span<PI>             indices;
    Span<Scalar>         weights;
    Span<Point>          points;
};

#define DTP template<class Scalar,int nb_dims>
#define UTP PointTreeWithValues<Scalar,nb_dims>

DTP void UTP::get_otps( Vec<std::tuple<Point,Scalar,PI>> &otps, const Point &p0, PI i0 ) {
    otps.clear();
    for( PI n1 = 0, nc = points.size(); n1 < nc; ++n1 )
        if ( indices[ n1 ] != i0 )
            otps << std::tuple<Point,Scalar,PI>{ points[ n1 ], weights[ n1 ], indices[ n1 ] };
    std::sort( otps.begin(), otps.end(), [&]( const auto &a, const auto &b ) {
        return norm_2_p2( std::get<0>( a ) - p0 ) < norm_2_p2( std::get<0>( b ) - p0 );
    } );
}


#undef DTP
#undef UTP
