#pragma once

#include "support/type_name.h"
#include "support/CC_DT.h"
#include "support/Vec.h"

#define Edge CC_DT( Edge )

/**
 * @brief 
 * 
 */
class Edge { STD_METIL_TYPE_INFO( Edge, "", num_cuts, vertices )
public:
    using VecNC      = Vec<PI,std::max(PD_DIM-1,0)>;
    using VecVI      = Vec<PI,2>;

    VecNC num_cuts;  ///<
    VecVI vertices;
};
