#include "PointTree_AABB.h"

PointTree::~PointTree() {
}

PointTree *PointTree::New( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights ) {
    return new PointTree_AABB( cp, points, weights );
}

Str PointTree::type_name() {
    return "PointTree";
}
