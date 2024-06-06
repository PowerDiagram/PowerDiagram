#pragma once

#include "support/type_name.h"
#include "Point.h"

#define Cut CC_DT( Cut )

/**
 * @brief 
 * 
 */
class Cut { STD_METIL_TYPE_INFO( Cut, "", n_index, dir, sp )
public:
    SI     n_index;
    Point  dir;
    Scalar sp;
};
