#include <tl/support/containers/operators/mean.h>
#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

#include <fstream>

//amgcl::profiler<> *prof;

template<class Scalar,int nb_dims>
void test_astro( std::string filename, PI mp ) {
    using Point = Vec<Scalar,nb_dims>;

    // load points
    std::ifstream f( filename );
    Vec<Point> positions;
    Vec<Scalar> weights;
    if ( filename.ends_with( ".xyz32.bin" ) ) {
        for( PI i = 0; i < ( 1 << 22 ); ++i ) {
            float x, y, z;
            f.read( (char *)&x, sizeof( x ) );
            f.read( (char *)&y, sizeof( y ) );
            f.read( (char *)&z, sizeof( z ) );
            if ( ! f )
                break;
            positions << Point{ x, y, z };
            weights << 0;
        }
    } else {
        for( PI i = 0; ; ++i ) {
            double x, y, z;
            f >> x >> y >> z;
            if ( ! f )
                break;
            positions << Point{ x, y, z };
            weights << 0;
        }
    }
    // P( positions.size() );

    // boundaries
    Vec<Scalar> bnd_offs;
    Vec<Point> bnd_dirs;
    for( PI d = 0; d < nb_dims; ++d ) {
        Point p( FromItemValue(), 0 ); p[ d ] = +1;
        Point q( FromItemValue(), 0 ); q[ d ] = -1;
        bnd_offs << 1 << 0;
        bnd_dirs << p << q;
    }

    // prof->tic( "setup" );
    PointTreeCtorParms cp{ .max_nb_points = mp };
    PowerDiagram<Scalar,nb_dims> pd( cp, std::move( positions ), std::move( weights ), bnd_dirs, bnd_offs );
    // prof->toc( "setup" );

    // prof->tic( "cell" );

    auto tStartSteady = std::chrono::steady_clock::now();

    Vec<Scalar> volumes( FromSizeAndItemValue(), pd.max_nb_threads(), 0 );
    pd.for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell, int num_thread ) {
        volumes[ num_thread ] += cell.measure();
    } );
    // prof->toc( "cell" );

    auto tEndSteady = std::chrono::steady_clock::now();
    std::chrono::nanoseconds diff = tEndSteady - tStartSteady;
    std::cout << diff.count() / 1e6 << "ms vol:" << sum( volumes ) << " mp:" << mp << std::endl;

    // std::cout << *prof << std::endl;
    // P( mp, sum( volumes ) );
}


TEST_CASE( "Astro 3D", "" ) {
    //prof = new amgcl::profiler<>( "sdot" );
    // test_astro<double,3>( "/home/leclerc/test_amg/16M.xyz32.bin" );
    for( PI i = 13; i <= 28; i += 2 )
        test_astro<double,3>( "/home/leclerc/test_amg/Points_1000000.xyz", i );
}
