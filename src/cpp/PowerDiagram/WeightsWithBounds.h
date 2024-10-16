#pragma once

#include "Cell.h"

namespace power_diagram {
PD_CLASS_DECL_AND_USE( WeightsWithBounds );

/**
 * @brief Abstract class to describe a set of point in connected sub-regions
 * 
 */
class PD_NAME( WeightsWithBounds ) { STD_TL_TYPE_INFO( WeightsWithBounds, "" )
public:
    virtual bool may_intersect( const Cell &cell, const Cell &beam ) const { return true; }

    Span<TF>     weights;
};

} // namespace power_diagram
