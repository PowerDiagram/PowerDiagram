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
    using Periodicity           = Vec<Opt<Vec<TF,2>>,nb_dims>;
    using Nd                    = Vec<PI,nb_dims>;

    /**/  PD_NAME( RegularGrid )( const DiracVec &dv, const Periodicity &periodicity );

    Pt    min_pos;              ///<
    Pt    max_pos;              ///<
    Nd    nb_divs;              ///<
};

} // namespace power_diagram
