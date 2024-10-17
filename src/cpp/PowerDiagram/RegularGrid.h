#pragma once

#include <tl/support/containers/Opt.h>
#include "PowerDiagram/Config.h"
#include "PavingWithDiracs.h"
#include "DiracVec.h"

namespace power_diagram {
PD_CLASS_DECL_AND_USE( RegularGrid );

/**
 * @brief Abstract class to describe a set of point in connected sub-regions
 * 
 */
class PD_NAME( RegularGrid ) : public PavingWithDiracs { STD_TL_TYPE_INFO( RegularGrid, "" )
public:
    using        Periodicity           = Vec<Opt<Vec<TF,2>>,nb_dims>;
    using        Nd                    = Vec<PI,nb_dims>;
       
    /**/         PD_NAME( RegularGrid )( const DiracVec &dv, const Periodicity &periodicity );

    virtual int  for_each_cell         ( const Cell &base_cell, const WeightsWithBounds &wwb, const std::function<void( Cell &cell, int num_thread )> &f ) override; ///< return 0 or an error code

    void         make_cuts_from        ( PI b0, PI n0, Cell &cell, Vec<PI> &buf, const WeightsWithBounds &wwb );
    PI           end_index             () const;
    PI           index                 ( const Pt &pos, int dim ) const;
    PI           index                 ( const Pt &pos ) const;
     
    Pt           min_pos;              ///<
    Pt           max_pos;              ///<
    Nd           nb_divs;              ///<
    Pt           steps;                ///<
     
    Vec<PI>      offsets;              ///<
    Vec<Pt>      points;               ///<
    Vec<PI>      inds;                 ///<
};

} // namespace power_diagram
