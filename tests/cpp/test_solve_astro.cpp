#include <tl/support/ASSERT.h>
#include "SdotSolver.h"
#include "catch_main.h"
#include <cstdlib>
#include <fstream>

void bench( Str filename ) {
    using Pt = SdotSolver::Pt;
    using TF = SdotSolver::TF;

    SdotSolver solver;

    // load points
    std::ifstream f( filename );
    if ( filename.ends_with( ".xyz32.bin" ) ) {
        for( PI i = 0; ; ++i ) {
            float x, y, z;
            f.read( (char *)&x, sizeof( x ) );
            f.read( (char *)&y, sizeof( y ) );
            f.read( (char *)&z, sizeof( z ) );
            if ( ! f )
                break;
            solver.positions << Pt{ x, y, z };
        }
    } else if ( filename.ends_with( ".xyz" ) ) {
        for( PI i = 0; ; ++i ) {
            double x, y, z;
            f >> x >> y >> z;
            if ( ! f )
                break;
            solver.positions << Pt{ x, y, z };
        }
    } else {
        for( PI i = 0; i < 100; ++i )
            solver.positions << Pt{ TF( rand() ) / RAND_MAX, TF( rand() ) / RAND_MAX, TF( rand() ) / RAND_MAX };
    }
    P( solver.positions.size() );

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

        for( coeff = 1.0; ; coeff /= 2 ) {
            ASSERT( coeff >= 1e-9 );
            prop = weights - coeff * sol;
            sys = solver.system( prop );
            if ( ! sys.void_cell )
                break;
        }

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
    bench( "Points_1000000.xyz" );
}
