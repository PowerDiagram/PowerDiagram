#include <tl/support/operators/sum.h>
#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"
#include <fstream>

void bench( Str filename ) {
    constexpr int nd = 3;
    using TF = double;

    struct Config { using Scalar = TF; enum { nb_dims = nd, use_AABB_bounds_on_cells = 0 }; };
    using Pt = Vec<TF,nd>;

    // load points
    std::ifstream f( filename );
    Vec<Pt> positions;
    Vec<TF> weights;
    if ( filename.ends_with( ".xyz32.bin" ) ) {
        for( PI i = 0; ; ++i ) {
            float x, y, z;
            f.read( (char *)&x, sizeof( x ) );
            f.read( (char *)&y, sizeof( y ) );
            f.read( (char *)&z, sizeof( z ) );
            if ( ! f )
                break;
            positions << Pt{ x, y, z };
            weights << 0;
        }
    } else {
        for( PI i = 0; i < 16777216; ++i ) {
            positions << Pt{ rand() / RAND_MAX, rand() / RAND_MAX, rand() / RAND_MAX };
            weights << 0;
        }
        // for( PI i = 0; ; ++i ) {
        //     double x, y, z;
        //     f >> x >> y >> z;
        //     if ( ! f )
        //         break;
        //     positions << Pt{ x, y, z };
        //     weights << 0;
        // }
    }
    P( positions.size() );

    // boundaries
    Vec<TF> bnd_offs;
    Vec<Pt> bnd_dirs;
    for( PI d = 0; d < nd; ++d ) {
        Pt p( FromItemValue(), 0 ); p[ d ] = +1;
        Pt q( FromItemValue(), 0 ); q[ d ] = -1;
        bnd_offs << 1 << 0;
        bnd_dirs << p << q;
    }

    // prof->tic( "setup" );
    PointTreeCtorParms cp{ .max_nb_points = 18 };
    PowerDiagram<Config> pd( cp, std::move( positions ), std::move( weights ), bnd_dirs, bnd_offs );
    // prof->toc( "setup" );

    // prof->tic( "cell" );

    auto t0 = std::chrono::steady_clock::now();

    Vec<TF> volumes( FromSizeAndItemValue(), pd.max_nb_threads(), 0 );
    Vec<PrevCutInfo<Config>> prev_cuts( FromSize(), pd.nb_cells() );
    pd.for_each_cell( [&]( Cell<Config> &cell, int num_thread ) {
        cell.get_prev_cut_info( prev_cuts[ cell.i0 ] );
        // volumes[ num_thread ] += cell.measure();
    } );

    auto t1 = std::chrono::steady_clock::now();
    std::cout << std::chrono::nanoseconds( t1 - t0 ).count() / 1e6 << "ms" << std::endl;

    pd.for_each_cell( [&]( Cell<Config> &cell, int num_thread ) {
    }, prev_cuts.data() );

    auto t2 = std::chrono::steady_clock::now();
    std::cout << std::chrono::nanoseconds( t2 - t1 ).count() / 1e6 << "ms" << std::endl;

    // P( sum( volumes ) );
}

TEST_CASE( "bench Astro 3D", "" ) {
    bench( "100M_60Mpch_jittered.xyz32.bin" );
}
