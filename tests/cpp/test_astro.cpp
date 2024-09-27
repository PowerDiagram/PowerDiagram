#include <tl/support/containers/operators/mean.h>
#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

#include <amgcl/profiler.hpp>
#include <fstream>

template<class Scalar,int nb_dims>
void test_astro( std::string filename = {} ) {
    using Point = Vec<Scalar,nb_dims>;

    // load points
    std::ifstream f( filename );
    Vec<Point> positions;
    Vec<Scalar> weights;
    Vec<PI> indices;
    for( PI i = 0; i < ( 1 << 15 ); ++i ) {
        float x, y, z;
        f.read( (char *)&x, sizeof( x ) );
        f.read( (char *)&y, sizeof( y ) );
        f.read( (char *)&z, sizeof( z ) );
        if ( ! f )
            break;
        positions << Point{ x, y, z };
        indices << weights.size();
        weights << 0;
    }

    // positions.resize( 1 << 11 );
    // weights.resize( 1 << 11 );
    // indices.resize( 1 << 11 );

    // boundaries
    Vec<Scalar> bnd_offs;
    Vec<Point> bnd_dirs;
    for( PI d = 0; d < nb_dims; ++d ) {
        Point p( FromItemValue(), 0 ); p[ d ] = +1;
        Point q( FromItemValue(), 0 ); q[ d ] = -1;
        bnd_offs << 1 << 0;
        bnd_dirs << p << q;
    }

    amgcl::profiler<> prof;

    prof.tic( "setup" );
    PointTreeCtorParms cp;
    PowerDiagram<Scalar,nb_dims> pd( cp, positions, weights, indices, bnd_dirs, bnd_offs );
    prof.toc( "setup" );

    prof.tic( "cell" );
    Vec<Scalar> volumes( FromSizeAndItemValue(), pd.max_nb_threads(), 0 );
    pd.for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell, int num_thread ) {
        volumes[ num_thread ] += cell.measure();
    } );
    prof.toc( "cell" );

    std::cout << prof << std::endl;
    P( sum( volumes ) );
}


TEST_CASE( "Astro 3D", "" ) {
    test_astro<double,3>( "/home/leclerc/test_amg/16M.xyz32.bin" );
}
