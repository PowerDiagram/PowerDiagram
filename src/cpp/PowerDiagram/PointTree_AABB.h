#pragma once

#include "PointTreeCtorParms.h"
#include "PointTree.h"
#include "Point.h"

#include "support/Span.h"

#define PointTree_AABB CC_DT( PointTree_AABB )

/**
*/
class PointTree_AABB : public PointTree {
public:
    using                PtPtr         = UniquePtr<PointTree>;

    /**/                 PointTree_AABB( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights );

    void                 init_children ( const PointTreeCtorParms &cp );
    void                 init_bounds   ( const PointTreeCtorParms &cp );
    static Str           type_name     ();
    virtual DisplayItem* display       ( DisplayItemFactory &df ) const;

    Vec<PtPtr>           children;
    Span<Scalar>         weights;
    Span<Point>          points;

    Scalar               min_offset_weights;
    Scalar               max_offset_weights;
    Point                coeff_weights;
    Point                min_pos;
    Point                max_pos;
};
