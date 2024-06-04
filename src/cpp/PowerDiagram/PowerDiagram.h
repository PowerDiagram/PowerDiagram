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
    /**/         PowerDiagram ( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights );
    /**/        ~PowerDiagram ();

    void         foe_each_cell( const std::function<void( const Cell &cell )> &f );
    DisplayItem *display      ( DisplayItemFactory &df ) const;

private:
    using        PtPtr        = UniquePtr<PointTree>;

    PtPtr        point_tree;  ///<
};
