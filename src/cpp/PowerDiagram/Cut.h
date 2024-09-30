#pragma once

#include <tl/support/type_info/type_name.h>
#include <tl/support/containers/Vec.h>
#include "CutType.h"

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class Cut { // STD_TL_TYPE_INFO( Cut, "", n_index, dir, sp )
public:
    using   Point    = Vec<Scalar,nb_dims>;

    /**/    Cut      ( CutType type, const Point &dir, Scalar sp, const Point &p1, Scalar w1, PI i1 ) : type( type ), dir( dir ), sp( sp ), p1( p1 ), w1( w1 ), i1( i1 ) {}
    /**/    Cut      () {}

    bool    is_inf   () const { return type == CutType::Infinity; }

    CutType type;
    Point   dir;
    Scalar  sp;

    Point   p1;
    Scalar  w1;
    PI      i1;
};
