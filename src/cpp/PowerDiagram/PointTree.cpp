#include "PointTree_AABB.h"

PointTree::PointTree( Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree *parent ) : indices( indices ), weights( weights ), points( points ), parent( parent ) {
}

PointTree::~PointTree() {
}

PointTree *PointTree::New( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree *parent ) {
    return new PointTree_AABB( cp, points, weights, indices, parent );
}

Str PointTree::type_name() {
    return "PointTree";
}
