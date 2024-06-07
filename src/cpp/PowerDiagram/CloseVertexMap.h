#pragma once

#include "support/UniquePtr.h"
#include "PointTree.h"
#include "Cell.h"

#define PowerDiagram CC_DT( PowerDiagram )

/**
 * @brief 
 * 
 */
class PowerDiagram {
public:
    /**/         PowerDiagram ( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, Span<Point> bnd_dirs, Span<Scalar> bnd_offs );

    static Str   type_name    ();

    void         for_each_cell( const std::function<void( const Cell &cell )> &f );
    DisplayItem *display      ( DisplayItemFactory &df ) const;

    Scalar       coeff_init_simplex;
private:
    using        PtPtr        = UniquePtr<PointTree>;

    PtPtr        point_tree;  ///<
    Span<Point>  bnd_dirs;    ///<
    Span<Scalar> bnd_offs;    ///<
};
