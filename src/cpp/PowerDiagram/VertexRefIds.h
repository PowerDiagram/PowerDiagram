#pragma once

#include <tl/support/Displayer.h>
#include "Config.h"

/**
 * @brief
 *
 */
class PD_NAME( VertexRefs ) { STD_TL_TYPE_INFO( VertexRefs, "", num_cuts ) //
public:
    using   NumCuts = Vec<PI32,POWER_DIAGRAM_CONFIG_NB_DIM>;
    //using Pad  = Vec<char,ceil()-sizeof(Inds)>;

    NumCuts num_cuts; 
    //Pad   _pad;
};

