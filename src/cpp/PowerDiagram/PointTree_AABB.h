#pragma once

#include "PointTreeWithValues.h"

/**
*/
template<class Scalar,int nb_dims>
class PointTree_AABB : public PointTreeWithValues<Scalar,nb_dims> {
public:
    using                Point         = Vec<Scalar,nb_dims>;

    /**/                 PointTree_AABB( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree<Scalar,nb_dims> *parent, PI num_in_parent );

    void                 init_children ( const PointTreeCtorParms &cp );
    void                 init_bounds   ( const PointTreeCtorParms &cp );
    static Str           type_name     ();

    #ifndef AVOID_DISPLAY
    virtual void         display      ( Displayer &df ) const override;
    #endif

    Point                inv_sym       ( const Point &pt, int ) const { return pt; }

    virtual bool         may_intersect ( const Point &vertex, const Point &p0, Scalar w0 ) const override;
    virtual Point        min_point     () const override { return min_pos; }
    virtual Point        max_point     () const override { return max_pos; }

    Scalar               min_offset_weights;
    Scalar               max_offset_weights;
    Point                coeff_weights;
    int                  num_sym;
    Point                min_pos;
    Point                max_pos;
};

#include "PointTree_AABB.cxx"
