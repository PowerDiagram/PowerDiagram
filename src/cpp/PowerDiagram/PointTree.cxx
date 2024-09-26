#pragma once

#include "PointTree_AABB.h"

#define DTP template<class Scalar,int nb_dims>
#define UTP PointTree<Scalar,nb_dims>

DTP UTP::PointTree( PointTree<Scalar,nb_dims> *parent, PI num_in_parent ) : num_in_parent( num_in_parent ), parent( parent ) {
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

DTP Vec<UTP *> UTP::split( PI nb_sub_lists ) {
    return {};
}

#undef DTP
#undef UTP
