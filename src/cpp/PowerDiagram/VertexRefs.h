#pragma once

#include <tl/support/Displayer.h>
#include "Config.h"

namespace power_diagram {
PD_CLASS_DECL_AND_USE( VertexRefs );

/**
 * @brief
 *
 */
class PD_NAME( VertexRefs ) { STD_TL_TYPE_INFO( VertexRefs, "", num_cuts ) //
public:
    using   NumCuts = Vec<PI32,nb_dims>;
    //using Pad  = Vec<char,ceil()-sizeof(Inds)>;

    NumCuts num_cuts; 
    //Pad   _pad;
};

} // namespace power_diagram
