#pragma once

#include "Cell.h"

namespace sdot {
PD_CLASS_DECL_AND_USE( WeightsWithBounds );

/**
 * @brief Abstract class to describe a set of point in connected sub-regions
 * 
 */
class PD_NAME( WeightsWithBounds ) { STD_TL_TYPE_INFO( WeightsWithBounds, "" )
public:
    virtual     ~PD_NAME( WeightsWithBounds )() {}

    virtual bool may_intersect               ( const Cell &cell, const Cell &beam ) const { return true; }
    virtual TF   operator[]                  ( PI index ) const = 0;
               
    virtual void display                     ( Displayer &ds ) const { ds << "yo"; }
};

} // namespace sdot
