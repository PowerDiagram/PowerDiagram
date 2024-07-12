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

    /**/         Vertex     ( VecInd num_cuts = {}, Point pos = {} ) : num_cuts( num_cuts ), pos( pos ), op_id( 0 ) {}

    VecInd       num_cuts;  ///<
    Point        pos;       ///<

    mutable PI   op_id = 0; ///<
};
