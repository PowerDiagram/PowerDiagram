#pragma once

#include "PointTree.h"

/**
*/
template<class Scalar,int nb_dims>
class PointTreeWithValues : public PointTree<Scalar,nb_dims> {
public:
    using                Point              = PointTree<Scalar,nb_dims>::Point;

    /**/                 PointTreeWithValues( Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree<Scalar,nb_dims> *parent, PI num_in_parent ) : PointTree<Scalar,nb_dims>( parent, num_in_parent ), indices( indices ), weights( weights ), points( points ) {}

    virtual void         for_each_point     ( const std::function<void( const Point &p0, Scalar w0, PI i0, PI32 n1 )> &f, Span<PI32> sub_indices ) override {
         for( PI32 si : sub_indices )
            f( points[ si ], weights[ si ], indices[ si ], si );
    }
    virtual void         for_each_point     ( const std::function<void( Span<Point> p0s, Span<Scalar> w0s, Span<PI> i0s )> &f ) override { f( points, weights, indices ); }
    virtual PI           nb_seed_points     () const { return indices.size(); }

    Span<PI>             indices;
    Span<Scalar>         weights;
    Span<Point>          points;
};

// #define DTP template<class Scalar,int nb_dims>
// #define UTP PointTreeWithValues<Scalar,nb_dims>

// #undef DTP
// #undef UTP
