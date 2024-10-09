#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/Displayer.h>

/**
 * TS = Type of Scalar
 * 
 */
template<class TF,int nb_dims>
class CellVertex { STD_TL_TYPE_INFO( CellVertex, "", num_cuts, pos )
public:
    using    NumCuts    = Vec<PI32,nb_dims>;
    using    Pt         = Vec<TF,nb_dims>;

    /**/     CellVertex ( NumCuts num_cuts = {}, Pt pos = {} ) : num_cuts( num_cuts ), pos( pos ) {}

    NumCuts  num_cuts;  ///<
    Pt       pos;       ///<
    
    char     pad[ 4 ];
};
