#pragma once

#include "support/type_name.h"
#include "support/Vec.h"

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class Cut { STD_METIL_TYPE_INFO( Cut, "", n_index, dir, sp )
public:
    using  Point    = Vec<Scalar,nb_dims>;

    bool   is_inf   () const { return n_index < 0; }

    SI     n_index;
    Point  dir;
    Scalar sp;
};
