#pragma once

#include <tl/support/type_info/type_name.h>
#include <tl/support/containers/Vec.h>

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class Edge { // STD_TL_TYPE_INFO( Edge, "", num_cuts, vertices )
public:
    using VecNC      = Vec<PI,std::max(nb_dims-1,0)>;
    using VecVI      = Vec<PI,2>;

    /**/  Edge       ( VecNC num_cuts = {}, VecVI vertices = {} ) : num_cuts( num_cuts ), vertices( vertices ) {}

    VecNC num_cuts;  ///<
    VecVI vertices;
};
