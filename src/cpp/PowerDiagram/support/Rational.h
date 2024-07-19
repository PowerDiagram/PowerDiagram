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
inline auto *display( DisplayItemFactory &sr, const Rational &value ) { std::ostringstream ss; cln::fprint( ss, value ); return sr.new_number( CtType<Rational>(), { .numerator = ss.str() } ); }

inline FP64 conv( CtType<FP64>, const Rational &value ) { return cln::double_approx( value ); }

namespace Eigen {
template<> struct NumTraits<Rational> : GenericNumTraits<Rational> {
    typedef Rational Real;
    typedef Rational NonInteger;
    typedef Rational Nested;
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
