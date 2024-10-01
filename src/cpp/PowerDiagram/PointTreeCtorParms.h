#pragma once

#include <tl/support/common_types.h>

struct PointTreeCtorParms {
    PI   max_nb_points  = 20;
    bool divide_by_nd   = false;
    bool make_histogram = true;
};
