#pragma once

#include "PointTree.h"

#define RemainingBoxes CC_DT( RemainingBoxes )

/**
 * @brief 
 * 
 */
class RemainingBoxes { STD_METIL_TYPE_INFO( RemainingBoxes, "", remaining_boxes )
public:
    Vec<PointTree *> remaining_boxes;
};
