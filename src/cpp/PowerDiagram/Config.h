#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/P.h>

//    Flags to redefine
#ifndef POWER_DIAGRAM_CONFIG_SUFFIX
#define POWER_DIAGRAM_CONFIG_SUFFIX double_3
#endif

#ifndef POWER_DIAGRAM_CONFIG_SCALAR
#define POWER_DIAGRAM_CONFIG_SCALAR double
#endif

#ifndef POWER_DIAGRAM_CONFIG_NB_DIM
#define POWER_DIAGRAM_CONFIG_NB_DIM 3
#endif

#ifndef POWER_DIAGRAM_CONFIG_AABB_BOUNDS_ON_CELLS
#define POWER_DIAGRAM_CONFIG_AABB_BOUNDS_ON_CELLS 0
#endif

#ifndef POWER_DIAGRAM_CONFIG_PHASE_OF_SPS
#define POWER_DIAGRAM_CONFIG_PHASE_OF_SPS 1
#endif

// helper for PD_NAME
#define PD_CONCAT_MACRO( A, B ) PD_CONCAT_MACRO_IND( A, B)
#define PD_CONCAT_MACRO_IND( A, B ) A##_##B

/// concatenation with $POWER_DIAGRAM_CONFIG_SUFFIX
#define PD_NAME( EXPR ) PD_CONCAT_MACRO( EXPR, POWER_DIAGRAM_CONFIG_SUFFIX )

///
#define PD_CLASS_DECL_AND_USE( NAME ) class PD_NAME( NAME ); using NAME = PD_NAME( NAME );

// common types and values
namespace power_diagram {
    // synonyms 
    static constexpr PI nb_dims = POWER_DIAGRAM_CONFIG_NB_DIM;
    using               TF      = POWER_DIAGRAM_CONFIG_SCALAR;

    // deduced types
    using               Pt      = Vec<TF,nb_dims>; ///< Point
}
