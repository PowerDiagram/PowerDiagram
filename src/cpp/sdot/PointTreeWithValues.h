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

    virtual void         make_prev_cuts     ( Cell<Config> &cell, PI32 cuts ) override;
    virtual void         for_each_point     ( const std::function<void( Span<Pt> p0s, Span<TF> w0s, Span<PI> i0s )> &f ) override { f( points, weights, indices ); }
    virtual PI           nb_seed_points     () const { return indices.size(); }
-DSDOT_CONFIG_SUFFIX=PD_SUFFIX_double_3
-DSDOT_CONFIG_SCALAR=double
-DSDOT_CONFIG_NB_DIM=3

#undef UTP
