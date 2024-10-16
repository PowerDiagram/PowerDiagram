#pragma once

#include <tl/support/containers/Vec.h>

//    Flags to define
//
//    POWER_DIAGRAM_CONFIG_SUFFIX = double_3
//    POWER_DIAGRAM_CONFIG_SCALAR = double
//    POWER_DIAGRAM_CONFIG_NB_DIM = 3

#define PD_NAME( EXPR ) EXPR##_##POWER_DIAGRAM_CONFIG_SUFFIX

// common types and values
namespace power_diagram {
    // synonyms 
    static constexpr PI nb_dims = POWER_DIAGRAM_CONFIG_NB_DIM;
    using               TF      = POWER_DIAGRAM_CONFIG_SCALAR;

    // deduced types
    using               Pt      = Vec<TF,nb_dims>; ///< Point
}
