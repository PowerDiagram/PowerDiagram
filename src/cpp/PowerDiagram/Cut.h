#pragma once

#include <tl/support/type_info/type_name.h>
#include <tl/support/containers/Vec.h>

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class Cut { // STD_TL_TYPE_INFO( Cut, "", n_index, dir, sp )
public:
    using  Point    = Vec<Scalar,nb_dims>;

    /**/   Cut      ( SI n_index = -1, Point dir = {}, Scalar sp = {} ) : n_index( n_index ), dir( dir ), sp( sp ) {}
    bool   is_inf   () const { return n_index < 0; }

    SI     n_index;
    Point  dir;
    Scalar sp;
};
