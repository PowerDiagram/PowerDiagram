#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/Displayer.h>
#include "CutType.h"

/**
 * @brief 
 * 
 */
template<class TS,int nb_dims>
class CellCut { STD_TL_TYPE_INFO( Cut, "", type, dir, off, p1, w1, i1 )
public:
    using      Point    = Vec<TS,nb_dims>;

    /**/       CellCut  ( CutType type, const Point &dir, TS off, const Point &p1, TS w1, PI i1 ) : type( type ), dir( dir ), off( off ), p1( p1 ), w1( w1 ), i1( i1 ) {}
    /**/       CellCut  () {}

    void       display  ( Displayer &ds ) const { ds.start_object(); ds << dir << off; ds.end_object(); }
    bool       is_inf   () const { return type == CutType::Infinity; }

    CutType    type;
    Point      dir;
    TS         off;

    Point      p1;
    TS         w1;
    PI         i1;
};
