#include <tl/support/operators/mean.h>
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

    auto t0 = std::chrono::steady_clock::now();

    Vec<Scalar> nb_vertices( FromSizeAndItemValue(), pd.nb_cells() );
    Vec<Scalar> nb_cuts( FromSizeAndItemValue(), pd.nb_cells() );
    Vec<Scalar> nb_vertices_2( FromSizeAndItemValue(), pd.nb_cells() );
    Vec<Scalar> nb_cuts_2( FromSizeAndItemValue(), pd.nb_cells() );
    Vec<Scalar> volumes( FromSizeAndItemValue(), pd.max_nb_threads(), 0 );
    Vec<Vec<typename PowerDiagram<Scalar,nb_dims>::CutInfo>> prev_cuts( FromSize(), pd.nb_cells() );
    pd.for_each_cell( [&]( Cell<Scalar,nb_dims> &cell, int num_thread ) {
        nb_vertices[ cell.i0 ] = cell.capa_vertices();
        nb_cuts[ cell.i0 ] = cell.capa_cuts();

        cell.memory_compaction();

        nb_vertices_2[ cell.i0 ] = cell.capa_vertices();
        nb_cuts_2[ cell.i0 ] = cell.capa_cuts();

        volumes[ num_thread ] += cell.measure();

        // for( const CellCut<Scalar,nb_dims> &cut : cell.cuts )
        //     if ( cut.type == CutType::Dirac )
        //         prev_cuts[ cell.i0 ].push_back( cut.p1, cut.w1, cut.i1 );
    } );
    // prof->toc( "cell" );

    auto t1 = std::chrono::steady_clock::now();
    std::cout << std::chrono::nanoseconds( t1 - t0 ).count() / 1e6 << "ms vol:" << sum( volumes ) << " mp:" << mp << std::endl;

    pd.for_each_cell( [&]( Cell<Scalar,nb_dims> &cell, int num_thread ) {
        nb_vertices[ cell.i0 ] = cell.capa_vertices();
        nb_cuts[ cell.i0 ] = cell.capa_cuts();

        cell.memory_compaction();

        nb_vertices_2[ cell.i0 ] = cell.capa_vertices();
        nb_cuts_2[ cell.i0 ] = cell.capa_cuts();

        volumes[ num_thread ] += cell.measure();
    }, prev_cuts.data() );
    // prof->toc( "cell" );

    auto t2 = std::chrono::steady_clock::now();
    std::cout << std::chrono::nanoseconds( t2 - t1 ).count() / 1e6 << "ms vol:" << sum( volumes ) << " mp:" << mp << std::endl;

    // std::cout << *prof << std::endl;
    // P( mp, sum( volumes ) );
    P( mean( nb_vertices ) );
    P( mean( nb_cuts ) );
    P( mean( nb_vertices_2 ) );
    P( mean( nb_cuts_2 ) );
}


TEST_CASE( "Astro 3D", "" ) {
    //prof = new amgcl::profiler<>( "sdot" );
    // test_astro<double,3>( "/home/leclerc/test_amg/16M.xyz32.bin" );
    for( PI i = 18; i <= 18; i += 1 )
        test_astro<double,3>( "Points_1000000.xyz", i );
}
