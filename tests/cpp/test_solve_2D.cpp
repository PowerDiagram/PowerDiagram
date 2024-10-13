#include <tl/support/operators/mean.h>
#include <tl/support/operators/max.h>
#include <tl/support/operators/sum.h>
#include <tl/support/ASSERT.h>
#include "SdotSolver.h"
#include "catch_main.h"
#include <cstdlib>
#include <fstream>

#include <matplotlibcpp.h>

void bench( Str filename ) {
    using Pt = SdotSolver::Pt;
    using TF = SdotSolver::TF;

    SdotSolver solver;

    // load points
    for( PI i = 0; i < 10; ++i )
        solver.positions << Pt{ 0.5 * TF( rand() ) / RAND_MAX, TF( rand() ) / RAND_MAX };

    // boundaries
    for( PI d = 0; d < 3; ++d ) {
        Pt p( FromItemValue(), 0 ); p[ d ] = +1;
        Pt q( FromItemValue(), 0 ); q[ d ] = -1;
        solver.bnd_offs << 1 << 0;
        solver.bnd_dirs << p << q;
    }

    Vec<TF> target_masses( FromSizeAndItemValue(), solver.nb_cells(), TF( 1 ) / solver.nb_cells() );
    Vec<TF> weights( FromSizeAndItemValue(), solver.nb_cells(), 0.0 );

    // first system
    auto sys = solver.system( weights );

    Vec<TF> errs;
    Vec<TF> errm;
    Vec<TF> prop;
    TF coeff = 0.0;
    for( int iter = 0; sys.max_diff > 0.01 / solver.nb_cells(); ++iter ) {
        P( 1e9 * sys.S, sys.max_diff * solver.nb_cells(), coeff );

        auto sol = sys.solve();

        auto mi = min( sol );
        auto ma = max( sol );
        auto de = ( ma - mi ) / 2;

        sol = sol - ( mi + ma ) / 2;

        // matplotlibcpp::hist( std::vector<double>( sol.begin(), sol.end() ), 256 );
        // matplotlibcpp::show();

        for( coeff = 1.0; ; coeff *= 1.5 ) {
            ASSERT( coeff >= 1e-9 );
            prop = weights - coeff * sol;

            if ( coeff != 1 )
                for( PI i = 0; i < weights.size(); ++i )
                    prop[ i ] = weights[ i ] - std::clamp( sol[ i ], - de / coeff, + de / coeff );

            sys = solver.system( prop );
            if ( ! sys.void_cell )
                break;

            P( sum( ( solver.measures( prop ) == 0 ) * 1.0 ) / solver.nb_cells() );
        }


        // for( coeff = 1.0; ; coeff /= 2 ) {
        //     ASSERT( coeff >= 1e-9 );
        //     prop = weights - coeff * sol;

        //     if ( coeff != 1 )
        //         for( PI i = 0; i < weights.size(); ++i )
        //             prop[ i ] = weights[ i ] - clamph( sol[ i ], - coeff * dsl, + coeff * dsl ) );

        //     sys = solver.system( prop );
        //     if ( ! sys.void_cell )
        //         break;

        //     //P( sum( ( solver.measures( prop ) == 0 ) * 1.0 ) / solver.nb_cells() );
        // }

        weights = std::move( prop );

        errm.push_back( sys.max_diff );
        errs.push_back( sys.S );
    }

    P( errm );
    P( errs );

    // P( solver.measures( weights ) );

    // VtkOutput vo;
    // solver.display_vtk( vo, weights );
    // vo.save( "out.vtk" );
}

TEST_CASE( "bench Astro 3D", "" ) {
    // bench( "16M.xyz32.bin" );
    bench( "2M.xyz32.bin" );
    // bench( "Points_1000000.xyz" );
}
