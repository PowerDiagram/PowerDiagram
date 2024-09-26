#pragma once

#include <tl/support/memory/UniquePtr.h>
#include <tl/support/containers/Vec.h>
#include "PointTreeCtorParms.h"

#ifndef AVOID_DISPLAY
#include <tl/support/Displayer.h>
#endif

/**
*/
template<class Scalar,int nb_dims>
class PointTree {
public:
    using                PtUPtr        = UniquePtr<PointTree<Scalar,nb_dims>>;
    using                Point         = Vec<Scalar,nb_dims>;

    /**/                 PointTree     ( PointTree *parent, PI num_in_parent );
    virtual             ~PointTree     ();

    static Str           type_name     ();
    static PointTree*    New           ( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree *parent, PI num_in_parent );

    #ifndef AVOID_DISPLAY
    virtual void         display       ( TL_NAMESPACE::Displayer &df ) const = 0;
    #endif

    virtual void         for_each_point( const std::function<void( const Point &p0, const Scalar &w0, const PI i0 )> &f ) = 0;

    virtual PI           nb_seed_points() const = 0;
    virtual bool         may_intersect ( const Point &vertex, const Point &p0, Scalar w0 ) const = 0;
    PointTree*           first_leaf    (); ///<
    PointTree*           next_leaf     (); ///<
    virtual Point        min_point     () const = 0;
    virtual Point        max_point     () const = 0;
    bool                 is_a_leaf     () const;

    Vec<PointTree *>     split         ( PI nb_sub_lists ); ///< returns leaf bounds for an equal splitting (+ nullptr at the end)

    PI                   num_in_parent;
    Vec<PtUPtr>          children;
    PointTree*           parent;
};

#include "PointTree.cxx" // IWYU pragma: export
