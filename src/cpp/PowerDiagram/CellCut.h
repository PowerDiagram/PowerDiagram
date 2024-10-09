#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/Displayer.h>
#include "CutType.h"

template<class Config> class PointTree;

/**
 * @brief 
 * 
 */
template<class Config>
class CellCut { STD_TL_TYPE_INFO( Cut, "", type, dir, off, i1 ) // , p1, w1
public:
    static constexpr int nb_dims  = Config::nb_dims;
    using                TF       = Config::Scalar;

    using                Pt       = Vec<TF,nb_dims>;
    using                Ptr      = PointTree<Config>;
          
    /**/                 CellCut  ( CutType type, const Pt &dir, TF off, const Pt &p1, TF w1, PI i1, PointTree<Config> *ptr, PI num_in_ptr ) : type( type ), dir( dir ), off( off ), p1( p1 ), w1( w1 ), i1( i1 ), ptr( ptr ), num_in_ptr( num_in_ptr ) {}
    /**/                 CellCut  () {}
          
    bool                 is_inf   () const { return type == CutType::Infinity; }
          
    CutType              type;
    Pt                   dir;
    TF                   off;
          
    Pt                   p1;
    TF                   w1;
    PI                   i1;
          
    Ptr*                 ptr;
    PI                   num_in_ptr;
};
