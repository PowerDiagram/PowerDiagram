#pragma once

#include <tl/support/memory/UniquePtr.h>
#include <tl/support/containers/Vec.h>
#include "PointTreeCtorParms.h"
#include "Cell.h"

#ifndef AVOID_DISPLAY
#include <tl/support/Displayer.h>
#endif

/**
*/
template<class Config>
class PointTree {
public:
    static constexpr int nb_dims       = Config::nb_dims;
    using                TF            = Config::Scalar;

    using                PtUPtr        = UniquePtr<PointTree<Config>>;
    using                Pt            = Vec<TF,nb_dims>;

    /**/                 PointTree     ( PointTree *parent, PI num_in_parent );
    virtual             ~PointTree     ();

    static Str           type_name     ();
    static PointTree*    New           ( const PointTreeCtorParms &cp, Span<Pt> points, Span<TF> weights, Span<PI> indices, PointTree *parent, PI num_in_parent );

    #ifndef AVOID_DISPLAY
    virtual void         display       ( TL_NAMESPACE::Displayer &df ) const = 0;
    #endif

    virtual void         for_each_point( const std::function<void( const Pt &p0, TF w0, PI i0, PI32 n0 )> &f, Span<PI32> indices ) = 0;
    virtual void         for_each_point( const std::function<void( Span<Pt> p0s, Span<TF> w0s, Span<PI> i0s )> &f ) = 0;

    virtual PI           nb_seed_points() const = 0;
    virtual bool         may_intersect ( const Cell<Config> &cell ) const = 0;
    PointTree*           first_leaf    (); ///<
    PointTree*           next_leaf     (); ///<
    virtual Pt           min_point     () const = 0;
    virtual Pt           max_point     () const = 0;
    bool                 is_a_leaf     () const;

    Vec<PointTree *>     split         ( PI nb_sub_lists ); ///< returns leaf bounds for an equal splitting (+ nullptr at the end)

    PI                   num_in_parent;
    Vec<PtUPtr>          children;
    PointTree*           parent;
};

#include "PointTree.cxx" // IWYU pragma: export
