#include <tl/support/containers/Vec.h>
#include <sdot/BigRational.h>
#include "catch_main.h"
#include <Eigen/LU>

using namespace sdot;
using BR = BigRational;

TEST_CASE( "BigRational ctors", "" ) {
    P( BR() );
    P( BR( 2 ) / BR( 4 ) );
    P( BR( 3 ) / BR( 5, 2 ) );
    P( BR( -6 ) );

    P( BR( 2 ) + BR( 8 ) );
    P( ( BR( 2 ) + BR( 8 ) ).to<SI32>() );
    P( ( BR( 2 ) + BR( 8 ) ).to<FP64>() );

    P( BR( +0.5 ) );
    P( BR( -0.5 ) );
    P( BR( +2.5 ) );
    P( BR( -2.5 ) );
    P( BR( 10.0 ) );
    P( BR( 11.0 ) );
    P( BR( +0.0 ) );
    P( BR( -0.0 ) );

    P( abs( BR( +0.5 ) ) );
    P( abs( BR( -0.5 ) ) );

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
