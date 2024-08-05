#pragma once

#include "PointTree_AABB.h"

#define DTP template<class Scalar,int nb_dims>
#define UTP PointTree<Scalar,nb_dims>

DTP UTP::PointTree( Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree<Scalar,nb_dims> *parent, PI num_in_parent ) : num_in_parent( num_in_parent ), parent( parent ), indices( indices ), weights( weights ), points( points ) {
}

DTP UTP::~PointTree() {
}

DTP UTP *UTP::New( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree *parent, PI num_in_parent ) {
    return new PointTree_AABB( cp, points, weights, indices, parent, num_in_parent );
}

DTP bool UTP::leaf() const {
    return children.empty();
}

DTP Str UTP::type_name() {
    return "PointTree";
}

#undef DTP
#undef UTP
