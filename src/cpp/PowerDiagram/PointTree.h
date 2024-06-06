#pragma once

#include "support/display/DisplayItemFactory.h"
#include "PointTreeCtorParms.h"
#include "Point.h"

#define PointTree CC_DT( PointTree )
class CC_DT( RemainingBoxes );

/**
*/
class PointTree {
public:
    using                PtUPtr       = UniquePtr<PointTree>;

    /**/                 PointTree    ( Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree *parent );
    virtual             ~PointTree    ();

    static Str           type_name    ();
    static PointTree*    New          ( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree *parent );

    virtual DisplayItem *display      ( DisplayItemFactory &df ) const = 0;
    virtual Scalar       radius       () const = 0;
    virtual Point        center       () const = 0;
    bool                 leaf         () const;


    Vec<PtUPtr>          children;
    PointTree*           parent;

    Span<PI>             indices;
    Span<Scalar>         weights;
    Span<Point>          points;
};