#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/Displayer.h>

/**
 * @brief 
 * 
 */
template<class TS,int nb_dims>
class CellVertex { STD_TL_TYPE_INFO( CellVertex, "", num_cuts, pos )
public:
    using        NumCuts    = Vec<PI,nb_dims>;
    using        Point      = Vec<TS,nb_dims>;

    /**/         CellVertex ( NumCuts num_cuts = {}, Point pos = {} ) : num_cuts( num_cuts ), pos( pos ) {}

    NumCuts      num_cuts;  ///<
    Point        pos;       ///<

    mutable PI   op_id = 0; ///<
};
