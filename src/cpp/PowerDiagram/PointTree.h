#pragma once

#ifndef AVOID_DISPLAY
#include "support/display/DisplayItemFactory.h"
#endif

#include "PointTreeCtorParms.h"
#include "support/Vec.h"

/**
*/
template<class Scalar,int nb_dims>
class PointTree {
public:
    using                PtUPtr       = UniquePtr<PointTree<Scalar,nb_dims>>;
    using                Point        = Vec<Scalar,nb_dims>;

    /**/                 PointTree    ( Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree *parent );
    virtual             ~PointTree    ();

    static Str           type_name    ();
    static PointTree*    New          ( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree *parent );

    #ifndef AVOID_DISPLAY
    virtual DisplayItem *display      ( DisplayItemFactory &df ) const = 0;
    #endif
    virtual Point        min_point    () const = 0;
    virtual Point        max_point    () const = 0;
    bool                 leaf         () const;


    Vec<PtUPtr>          children;
    PointTree*           parent;

    Span<PI>             indices;
    Span<Scalar>         weights;
    Span<Point>          points;
};

#include "PointTree.tcc"
