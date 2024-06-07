#pragma once

#include "PointTreeCtorParms.h"
#include "PointTree.h"

#include "support/Span.h"

/**
*/
template<class Scalar,int nb_dims>
class PointTree_AABB : public PointTree<Scalar,nb_dims> {
public:
    using                Point         = Vec<Scalar,nb_dims>;

    /**/                 PointTree_AABB( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree<Scalar,nb_dims> *parent );

    void                 init_children ( const PointTreeCtorParms &cp );
    void                 init_bounds   ( const PointTreeCtorParms &cp );
    static Str           type_name     ();

    virtual DisplayItem* display       ( DisplayItemFactory &df ) const override;
    virtual Scalar       radius        () const override { return max( max_pos - min_pos ); }
    virtual Point        center        () const override { return ( max_pos + min_pos ) / 2; }

    Scalar               min_offset_weights;
    Scalar               max_offset_weights;
    Point                coeff_weights;
    Point                min_pos;
    Point                max_pos;
};

#include "PointTree_AABB.tcc"
