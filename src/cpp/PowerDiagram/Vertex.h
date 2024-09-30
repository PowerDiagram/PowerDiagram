#pragma once

#include <tl/support/type_info/type_name.h>
#include <tl/support/containers/Vec.h>

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class Vertex { // STD_TL_TYPE_INFO( Vertex, "", num_cuts, pos )
public:
    using        NumCuts    = Vec<PI,nb_dims>;
    using        Point      = Vec<Scalar,nb_dims>;

    /**/         Vertex     ( NumCuts num_cuts = {}, Point pos = {} ) : num_cuts( num_cuts ), pos( pos ) {}

    NumCuts      num_cuts;  ///<
    Point        pos;       ///<

    mutable PI   op_id = 0; ///<
};
