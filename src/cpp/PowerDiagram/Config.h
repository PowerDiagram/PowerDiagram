#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/P.h>

//    Flags to redefine
#ifndef POWER_DIAGRAM_CONFIG_suffix
#define POWER_DIAGRAM_CONFIG_suffix double_3
#endif

#ifndef POWER_DIAGRAM_CONFIG_scalar_type
#define POWER_DIAGRAM_CONFIG_scalar_type double
#endif

#ifndef POWER_DIAGRAM_CONFIG_nb_dims
#define POWER_DIAGRAM_CONFIG_nb_dims 3
#endif

#ifndef POWER_DIAGRAM_CONFIG_AABB_BOUNDS_ON_CELLS
#define POWER_DIAGRAM_CONFIG_AABB_BOUNDS_ON_CELLS 0
#endif

#ifndef POWER_DIAGRAM_CONFIG_PHASE_OF_SPS
#define POWER_DIAGRAM_CONFIG_PHASE_OF_SPS 1
#endif

// helper for PD_NAME
#define PD_CONCAT_MACRO_( A, B ) A##_##B
#define PD_CONCAT_MACRO( A, B ) PD_CONCAT_MACRO_( A, B)

#define PD_STR_CONCAT_MACRO_( A, B ) #A "_" #B
#define PD_STR_CONCAT_MACRO( A, B ) PD_STR_CONCAT_MACRO_( A, B )

/// concatenation with $POWER_DIAGRAM_CONFIG_suffix
#define PD_NAME( EXPR ) PD_CONCAT_MACRO( EXPR, POWER_DIAGRAM_CONFIG_suffix )

///
#define PD_STR( EXPR ) PD_STR_CONCAT_MACRO( EXPR, POWER_DIAGRAM_CONFIG_suffix )

///
#define PD_CLASS_DECL_AND_USE( NAME ) class PD_NAME( NAME ); using NAME = PD_NAME( NAME );

// common types and values
namespace power_diagram {
    // synonyms 
    static constexpr PI nb_dims = POWER_DIAGRAM_CONFIG_nb_dims;
    using               TF      = POWER_DIAGRAM_CONFIG_scalar_type;

    // deduced types
    using               Pt      = Vec<TF,nb_dims>; ///< Point
}
