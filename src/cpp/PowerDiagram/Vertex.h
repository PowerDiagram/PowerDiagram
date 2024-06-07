#pragma once

#include "support/type_name.h"
#include "support/Vec.h"

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class Vertex { STD_METIL_TYPE_INFO( Vertex, "", num_cuts, pos )
public:
    using        VecInd     = Vec<PI,nb_dims>;
    using        Point      = Vec<Scalar,nb_dims>;

    VecInd       num_cuts;  ///<
    Point        pos;       ///<

    mutable PI   op_id = 0; ///<
};
