#pragma once

#include "support/UniquePtr.h"
#include "PointTree.h"

#define PowerDiagram CC_DT( PowerDiagram )

/**
 * @brief 
 * 
 */
class PowerDiagram {
public:
    using        PtPtr       = UniquePtr<PointTree>;

    /**/         PowerDiagram( PtPtr &&point_tree );
    /**/        ~PowerDiagram();

    DisplayItem *display     ( DisplayItemFactory &df ) const;


private:    
    PtPtr        point_tree; ///<
};
