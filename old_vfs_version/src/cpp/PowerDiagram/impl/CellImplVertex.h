#pragma once

#include <vfs/containers/Vec.h>

///
template<class ST,int nb_dims>
struct CellImplVertex {
    using        VI         = Vec<PI,nb_dims>;
    using        Pt         = Vec<ST,nb_dims>;

    DisplayItem* display    ( Displayer &ds ) const{ return DS_OBJECT( Vertex, num_cuts, pos ); }

    VI           num_cuts;  ///<
    Pt           pos;       ///<

    mutable PI   op_id = 0; ///<
};

