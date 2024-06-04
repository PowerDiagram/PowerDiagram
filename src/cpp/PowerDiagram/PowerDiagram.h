#pragma once

#include "PointTreeCtorParms.h"
#include "PointTree.h"

#include "support/UniquePtr.h"

#define PowerDiagram CC_DT( PowerDiagram )

/**
 * @brief 
 * 
 */
class PowerDiagram {
public:
    /**/         PowerDiagram( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights );
    /**/        ~PowerDiagram();

    DisplayItem *display     ( DisplayItemFactory &df ) const;

private:
    using        PtPtr       = UniquePtr<PointTree>;

    PtPtr        point_tree; ///<
};
