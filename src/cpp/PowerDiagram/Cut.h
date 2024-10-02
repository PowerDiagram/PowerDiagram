#pragma once

#include <tl/support/type_info/type_name.h>
#include <tl/support/containers/Vec.h>
#include <tl/support/Displayer.h>
#include "CutType.h"

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class Cut { // STD_TL_TYPE_INFO( Cut, "", n_index, dir, sp )
public:
    using      Point    = Vec<Scalar,nb_dims>;

    void       display  ( Displayer &ds ) const { ds.start_object(); ds << dir << off; ds.end_object(); }
    bool       is_inf   () const { return type == CutType::Infinity; }

    CutType    type;
    int        prev;    ///< in active_cuts or inactive_cuts
    Point      dir;
    Scalar     off;

    Point      p1;
    Scalar     w1;
    PI         i1;

    mutable PI op_id    = 0; ///<
};
