#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/P.h>

//    Flags to redefine
#ifndef SDOT_CONFIG_suffix
#define SDOT_CONFIG_suffix double_3
#endif

#ifndef SDOT_CONFIG_scalar_type
#define SDOT_CONFIG_scalar_type double
#endif

#ifndef SDOT_CONFIG_nb_dims
#define SDOT_CONFIG_nb_dims 3
#endif

#ifndef SDOT_CONFIG_AABB_BOUNDS_ON_CELLS
#define SDOT_CONFIG_AABB_BOUNDS_ON_CELLS 0
#endif

#ifndef SDOT_CONFIG_PHASE_OF_SPS
#define SDOT_CONFIG_PHASE_OF_SPS 1
#endif

// helper for PD_NAME
#define PD_CONCAT_MACRO_( A, B ) A##_##B
#define PD_CONCAT_MACRO( A, B ) PD_CONCAT_MACRO_( A, B)

#define PD_STR_CONCAT_MACRO_( A, B ) #A "_" #B
#define PD_STR_CONCAT_MACRO( A, B ) PD_STR_CONCAT_MACRO_( A, B )

/// concatenation with $SDOT_CONFIG_suffix
#define PD_NAME( EXPR ) PD_CONCAT_MACRO( EXPR, SDOT_CONFIG_suffix )

///
#define PD_STR( EXPR ) PD_STR_CONCAT_MACRO( EXPR, SDOT_CONFIG_suffix )

///
#define PD_CLASS_DECL_AND_USE( NAME ) class PD_NAME( NAME ); using NAME = PD_NAME( NAME );

// common types and values
namespace sdot {
    // synonyms 
    static constexpr PI nb_dims = SDOT_CONFIG_nb_dims;
    using               TF      = SDOT_CONFIG_scalar_type;

    // deduced types
    using               Pt      = Vec<TF,nb_dims>; ///< Point
}
