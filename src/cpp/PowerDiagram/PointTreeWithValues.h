#pragma once

#include "PointTree.h"

/**
*/
template<class Config>
class PointTreeWithValues : public PointTree<Config> {
public:
    static constexpr int nb_dims            = Config::nb_dims;
    using                TF                 = Config::Scalar;

    using                Pt                 = PointTree<Config>::Pt;

    /**/                 PointTreeWithValues( Span<Pt> points, Span<TF> weights, Span<PI> indices, PointTree<Config> *parent, PI num_in_parent ) : PointTree<Config>( parent, num_in_parent ), indices( indices ), weights( weights ), points( points ) {}

    virtual void         for_each_point     ( const std::function<void( const Pt &p0, TF w0, PI i0, PI32 n1 )> &f, Span<PI32> sub_indices ) override { for( PI32 si : sub_indices ) f( points[ si ], weights[ si ], indices[ si ], si ); }
    virtual void         for_each_point     ( const std::function<void( Span<Pt> p0s, Span<TF> w0s, Span<PI> i0s )> &f ) override { f( points, weights, indices ); }
    virtual PI           nb_seed_points     () const { return indices.size(); }

    Span<PI>             indices;
    Span<TF>             weights;
    Span<Pt>             points;
};

// #define DTP template<class TF,int nb_dims>
// #define UTP PointTreeWithValues<TF,nb_dims>

// #undef DTP
// #undef UTP
