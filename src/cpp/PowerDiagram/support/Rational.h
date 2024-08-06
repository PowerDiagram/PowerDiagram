#pragma once

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Core>
#include <cln/rational_io.h>
#include <cln/rational.h>
#include <sstream>

#include "display/DisplayItemFactory.h"
#include "common_types.h"
#include "CtType.h"

//// nsmake lib_name cln

using Rational = cln::cl_RA;

inline auto type_name( CtType<Rational> ) { return "Rational"; }

inline FP64 conv( CtType<FP64>, const cln::cl_RA &value ) { return cln::double_approx( value ); }

inline auto display( DisplayItemFactory &sr, const Rational &value ) {
    std::ostringstream ss;
    cln::fprint( ss, value );
    return sr.new_number( { "Rational" }, {
        // TODO: find denominator
        .numerator = ss.str()
    } );
}

namespace Eigen {
template<> struct NumTraits<cln::cl_RA> : GenericNumTraits<cln::cl_RA> {
    typedef cln::cl_RA Real;
    typedef cln::cl_RA NonInteger;
    typedef cln::cl_RA Nested;
    static inline Real epsilon() { return 0; }
    static inline Real dummy_precision() { return 0; }
    static inline Real digits10() { return 0; }

    enum {
        IsInteger = 0,
        IsSigned = 1,
        IsComplex = 0,
        RequireInitialization = 1,
        ReadCost = 6,
        AddCost = 150,
        MulCost = 100
    };
};
}

BEG_METIL_NAMESPACE

inline Rational abs( const Rational &a ) { return cln::abs( a ); }
inline Rational abs( Rational &&a ) { return cln::abs( a ); }
inline Rational abs( Rational &a ) { return cln::abs( a ); }

inline auto epsilon( CtType<Rational> ) { return Rational( 0 ); }

END_METIL_NAMESPACE
