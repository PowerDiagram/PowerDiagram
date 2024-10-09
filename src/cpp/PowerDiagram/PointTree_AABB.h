#pragma once

#include "PointTreeWithValues.h"

/**
*/
template<class TF,int nb_dims>
class PointTree_AABB : public PointTreeWithValues<TF,nb_dims> {
public:
    using                Pt         = Vec<TF,nb_dims>;

    /**/                 PointTree_AABB( const PointTreeCtorParms &cp, Span<Pt> Pts, Span<TF> weights, Span<PI> indices, PointTree<TF,nb_dims> *parent, PI num_in_parent );

    void                 init_children ( const PointTreeCtorParms &cp );
    void                 init_bounds   ( const PointTreeCtorParms &cp );
    static Str           type_name     ();

    #ifndef AVOID_DISPLAY
    virtual void         display      ( Displayer &df ) const override;
    #endif

    Pt                   inv_sym       ( const Pt &pt, int ) const { return pt; }

    virtual bool         may_intersect ( const Cell<TF,nb_dims> &cell ) const override;
    virtual Pt           min_point     () const override { return min_pos; }
    virtual Pt           max_point     () const override { return max_pos; }

    TF                   min_offset_weights;
    TF                   max_offset_weights;
    Pt                   coeff_weights;
    int                  num_sym;
    Pt                   min_pos;
    Pt                   max_pos;
};

#include "PointTree_AABB.cxx"
