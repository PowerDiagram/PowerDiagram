#include <tl/support/containers/Vec.h>
#include <sdot/BigRational.h>
#include "catch_main.h"
#include <Eigen/LU>

using namespace sdot;
using BR = BigRational;

namespace Eigen {
    template<> struct NumTraits<BR> : GenericNumTraits<BR> {
        typedef BR Real;
        typedef BR NonInteger;
        typedef BR Nested;
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

TEST_CASE( "BigRational ctors", "" ) {
    P( BR() );
    P( BR( 2 ) / BR( 4 ) );
    P( BR( 3 ) / BR( 5, 2 ) );
    P( BR( -6 ) );

    P( BR( 2 ) + BR( 8 ) );
    P( ( BR( 2 ) + BR( 8 ) ).to<SI32>() );
    P( ( BR( 2 ) + BR( 8 ) ).to<FP64>() );

    P( BR::from_value( +0.5 ) );
    P( BR::from_value( -0.5 ) );
    P( BR::from_value( +2.5 ) );
    P( BR::from_value( -2.5 ) );
    P( BR::from_value( 10.0 ) );
    P( BR::from_value( 11.0 ) );
    P( BR::from_value( +0.0 ) );
    P( BR::from_value( -0.0 ) );

    P( abs( BR::from_value( +0.5 ) ) );
    P( abs( BR::from_value( -0.5 ) ) );

    BR a( 1, 2 );
    a += 5;
    P( a, a.to<FP64>() );

    a *= 2;
    P( a, a.to<FP64>() );
}

TEST_CASE( "BigRational matrix", "" ) {
    constexpr int nb_dims = 2;
    using TM = Eigen::Matrix<BR,nb_dims,nb_dims>;
    using TV = Eigen::Matrix<BR,nb_dims,1>;

    TM m;
    TV v;
    m.coeffRef( 0, 0 ) = 5;
    m.coeffRef( 0, 1 ) = 0;
    m.coeffRef( 1, 0 ) = 0;
    m.coeffRef( 1, 1 ) = 7;
    v[ 0 ] = 2;
    v[ 1 ] = 3;

    Eigen::FullPivLU<TM> lu( m );
    Vec<BR,nb_dims> X = lu.solve( v );

    P( X );
}
