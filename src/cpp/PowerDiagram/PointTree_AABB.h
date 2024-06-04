#pragma once

#include "support/Span.h"
#include "PointTree.h"
#include "Point.h"

#define PointTree_AABB CC_DT( PointTree_AABB )

/**
*/
class PointTree_AABB : public PointTree {
public:
    /**/                 PointTree_AABB( Span<Point> points, Span<Scalar> weights );

    virtual DisplayItem* display       ( DisplayItemFactory &df ) const;

    Vec<Scalar>          weights;
    Vec<Point>           points;
};
