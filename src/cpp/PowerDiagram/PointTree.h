#pragma once

#include "support/display/DisplayItemFactory.h"
#include "PointTreeCtorParms.h"
#include "Point.h"

#define PointTree CC_DT( PointTree )

/**
*/
class PointTree {
public:
    virtual             ~PointTree();

    static Str           type_name();
    static PointTree*    New      ( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights );

    virtual DisplayItem *display  ( DisplayItemFactory &df ) const = 0;
};
