#pragma once

#include "support/UniquePtr.h"
#include "PointTree.h"
#include "Cell.h"

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class PowerDiagram {
public:
    using        Point        = Vec<Scalar,nb_dims>;

    /**/         PowerDiagram ( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, Span<Point> bnd_dirs, Span<Scalar> bnd_offs );

    static Str   type_name    ();

    void         for_each_cell( const std::function<void( Cell<Scalar,nb_dims> &cell )> &f );
    #ifndef AVOID_DISPLAY
    DisplayItem *display      ( DisplayItemFactory &df ) const;
    #endif

    Scalar       coeff_init_simplex;
private:
    using        PtPtr        = UniquePtr<PointTree<Scalar,nb_dims>>;

    PtPtr        point_tree;  ///<
    Span<Point>  bnd_dirs;    ///<
    Span<Scalar> bnd_offs;    ///<
};

#include "PowerDiagram.tcc"
