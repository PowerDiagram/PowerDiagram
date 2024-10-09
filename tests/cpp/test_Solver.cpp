#include <tl/support/string/va_string.h>
#include <tl/support/operators/argmin.h>
#include <tl/support/operators/mean.h>
#include "SdotSolver.h"
#include "catch_main.h"

// #include "matplotlibcpp.h"

void test_solver( PI nb_cells, std::string filename = {} ) {
    constexpr PI nd = SdotSolver::Config::nb_dims;
    using TF = SdotSolver::TF;
    using Pt = SdotSolver::Pt;

    SdotSolver solver;

    // diracs
    for( PI i = 0; i < nb_cells; ++i ) {
        Pt p;
        for( PI d = 0; d < nd; ++d )
            p[ d ] = TF( rand() ) / RAND_MAX;
        solver.positions << p;
        solver.indices << i;
    }

    // boundaries
    for( PI d = 0; d < nd; ++d ) {
        Pt p( FromItemValue(), 0 ); p[ d ] = +1;
        Pt q( FromItemValue(), 0 ); q[ d ] = -1;
        solver.bnd_offs << 1 << 0;
        solver.bnd_dirs << p << q;
    }

    Vec<TF> weights( FromSizeAndItemValue(), solver.nb_cells(), 0.0 );

    P( solver.error( weights ) );

    P( solver.matrix_ap( weights ) );
    P( solver.matrix( weights ) );

    // Vec<TF> xs, ms;
    // auto mb = solver.measures( weights )[ solver.indices[ 0 ] ];
    // for( PI i = 0; i < 20; ++i ) {
    //     weights[ 0 ] = 0.5 + 1e-3 * i;
    //     xs << weights[ 0 ];
    //     ms << solver.measures( weights )[ solver.indices[ 0 ] ] - mb;

    //     VtkOutput vo;
    //     solver.display_vtk( vo, weights );
    //     vo.save( va_string( "out_$0.vtk", i ) );
    // }
    // P( mb );
    // P( xs );
    // P( ms );

    // for( int iter = 0; iter < 10; ++iter ) {
    //     Vec<TF> dir = solver.jacobi_dir( weights );

    //     auto err = solver.der_err( weights, dir );
    //     P( err );

    //     //P( err, err.derivative( 1 ) / err.derivative( 2 ) );
    //     std::vector<TF> xs;
    //     std::vector<TF> es;
    //     std::vector<TF> vs;
    //     for( PI i = 0; i < 2000; ++i ) {
    //         TF x = TF( i ) / 2000;
    //         xs.push_back( x );
    //         es.push_back( err[ 0 ] + err[ 1 ] * x /*+ err.derivative( 2 ) * pow( x, 2 ) / 2*/ );
    //         vs.push_back( solver.error( weights + x * dir ) );
    //     }
    //     P( xs[ argmin( vs ) ] );

    //     matplotlibcpp::plot( xs, es );
    //     matplotlibcpp::plot( xs, vs );
    //     matplotlibcpp::show();
    //     break;

    //     // TF alpha = err.derivative( 1 ) / err.derivative( 2 ) / 3;
    //     TF alpha = xs[ argmin( vs ) ];
    //     weights = weights + alpha * dir;

    //     // P( err.derivative( 0 ), argmin( vs ) );
    // }


    // if ( filename.size() ) {
    //     VtkOutput vo;
    //     solver.display_vtk( vo, weights );
    //     vo.save( filename );
    // }
}


TEST_CASE( "PowerDiagram 2D", "" ) {
    test_solver( 10, "out.vtk" );
}
