#pragma once

#include "support/type_name.h"
#include "Point.h"

#define Vertex CC_DT( Vertex )

/**
 * @brief 
 * 
 */
class Vertex { STD_METIL_TYPE_INFO( Vertex, "", num_cuts, pos )
public:
    using        VecInd = Vec<PI,PD_DIM>;

    VecInd       num_cuts;  ///<
    Point        pos;       ///<

    mutable PI   op_id = 0; ///<
};
