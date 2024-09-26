#pragma once

#include <tl/support/memory/UniquePtr.h>
#include "RemainingBoxes.h"
#include "PointTree.h"
#include "InfCell.h"
#include "Cell.h"

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class PowerDiagram {
public:
    using                   Point             = Vec<Scalar,nb_dims>;

    /**/                    PowerDiagram      ( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, Span<Point> bnd_dirs, Span<Scalar> bnd_offs );

    static Str              type_name         ();

    Point                   inv_sym           ( const Point &pt, int num_sym ) const { return pt; }
    Point                   sym               ( const Point &pt, int num_sym ) const { return pt; }

    int                     max_nb_threads    () const;
    void                    for_each_cell     ( const std::function<void( Cell<Scalar,nb_dims> &cell, int num_thread )> &f );
    void                    for_each_cell     ( const std::function<void( Cell<Scalar,nb_dims> &cell )> &f ); ///< version with a mutex lock for `f`
    auto                    cell_data_at      ( const Point &pt, Scalar probe_size ) const -> Vec<std::tuple<const Scalar *, const Point *, SI>>;
    auto                    cell_data_at      ( const Point &pt ) const -> Opt<std::tuple<const Scalar *, const Point *, SI>>;

    #ifndef AVOID_DISPLAY
    void                    display           ( Displayer &df ) const;
    #endif

private:
    using                   PtPtr             = UniquePtr<PointTree<Scalar,nb_dims>>;

    void                    make_intersections( auto &cell, const RemainingBoxes<Scalar,nb_dims> &rb_base );
    bool                    outside_cell      ( auto &cell, const RemainingBoxes<Scalar,nb_dims> &rb_base );

    InfCell<Scalar,nb_dims> base_inf_cell;    ///<
    Point                   min_box_pos;      ///<
    Point                   max_box_pos;      ///<
    PtPtr                   point_tree;       ///<
    Cell<Scalar,nb_dims>    base_cell;        ///<
    Span<Point>             bnd_dirs;         ///<
    Span<Scalar>            bnd_offs;         ///<
};

#include "PowerDiagram.cxx"
