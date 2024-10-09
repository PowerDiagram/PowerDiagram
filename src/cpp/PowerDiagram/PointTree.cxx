#pragma once

#include "PointTree_AABB.h"

#define DTP template<class TF,int nb_dims>
#define UTP PointTree<TF,nb_dims>

DTP UTP::PointTree( PointTree<TF,nb_dims> *parent, PI num_in_parent ) : num_in_parent( num_in_parent ), parent( parent ) {
}

DTP UTP::~PointTree() {
}

DTP UTP *UTP::New( const PointTreeCtorParms &cp, Span<Pt> points, Span<TF> weights, Span<PI> indices, PointTree *parent, PI num_in_parent ) {
    return new PointTree_AABB( cp, points, weights, indices, parent, num_in_parent );
}

DTP bool UTP::is_a_leaf() const {
    return children.empty();
}

DTP Str UTP::type_name() {
    return "PointTree";
}

DTP UTP *UTP::first_leaf() {
    if ( is_a_leaf() )
        return this;
    return children[ 0 ]->first_leaf();
}

DTP UTP *UTP::next_leaf() {
    PointTree *res = this;
    while ( true ) {
        if ( ! res->parent )
            return nullptr;
        if ( res->num_in_parent + 1 < res->parent->children.size() )
            return res->parent->children[ res->num_in_parent + 1 ]->first_leaf();
        res = res->parent;
    }
}

DTP Vec<UTP *> UTP::split( PI nb_sub_lists ) {
    Vec<PointTree *> res( FromSizeAndItemValue(), nb_sub_lists + 1, nullptr );
    const PI np = nb_seed_points();
    if ( np == 0 )
        return res;

    PointTree *pt = first_leaf();
    res[ 0 ] = pt;

    for( PI i = 1, acc = 0; i < nb_sub_lists; ++i ) {
        const PI np_bound = i * np / nb_sub_lists;

        while ( acc < np_bound ) {
            acc += pt->nb_seed_points();
            pt = pt->next_leaf();
            if ( ! pt )
                break;
        }

        res[ i ] = pt;
    }

    return res;
}

#undef DTP
#undef UTP
