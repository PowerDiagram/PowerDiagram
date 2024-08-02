#pragma once

#include "support/UniquePtr.h"
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

    void                    for_each_cell     ( const std::function<void( Cell<Scalar,nb_dims> &cell )> &f );

    #ifndef AVOID_DISPLAY
    DisplayItem*            display           ( DisplayItemFactory &df ) const;
    #endif

private:
    using                   PtPtr             = UniquePtr<PointTree<Scalar,nb_dims>>;

    void                    make_intersections( auto &cell, const RemainingBoxes<Scalar,nb_dims> &rb_base, PI n0 );
    bool                    outside_cell      ( auto &cell, const RemainingBoxes<Scalar,nb_dims> &rb_base, PI n0 );

    InfCell<Scalar,nb_dims> base_inf_cell;    ///<
    Point                   min_box_pos;      ///<
    Point                   max_box_pos;      ///<
    PtPtr                   point_tree;       ///<
    Cell<Scalar,nb_dims>    base_cell;        ///<
    Span<Point>             bnd_dirs;         ///<
    Span<Scalar>            bnd_offs;         ///<
};

#include "PowerDiagram.tcc"
