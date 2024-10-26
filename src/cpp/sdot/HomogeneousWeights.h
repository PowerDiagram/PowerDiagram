#pragma once

#include "WeightsWithBounds.h"

namespace power_diagram {
PD_CLASS_DECL_AND_USE( HomogeneousWeights );

/**
 * @brief Abstract class to describe a set of point in connected sub-regions
 * 
 */
class PD_NAME( HomogeneousWeights ) : public WeightsWithBounds { STD_TL_TYPE_INFO( HomogeneousWeights, "" )
public:
    /**/         PD_NAME( HomogeneousWeights )( TF weight_value = 1 ) : weight_value( weight_value ) {}

    virtual bool may_intersect                ( const Cell &cell, const Cell &beam ) const { return true; }
    virtual TF   operator[]                   ( PI index ) const { return weight_value; }

    TF           weight_value;
};

} // namespace power_diagram
