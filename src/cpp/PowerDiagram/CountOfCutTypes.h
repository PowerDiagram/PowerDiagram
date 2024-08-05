#pragma once

#include <PowerDiagram/support/display/DisplayItemFactory.h>

/**
*/
struct CountOfCutTypes { STD_METIL_TYPE_INFO( CountOfCutTypes, "", nb_ints, nb_bnds, nb_infs )
    int nb_ints = 0; ///<
    int nb_bnds = 0; ///<
    int nb_infs = 0; ///<
};
