#include <tl/support/operators/mean.h>
#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

#include <fstream>

//amgcl::profiler<> *prof;

template<class TF,int nd>
void test_astro( std::string filename, PI mp ) {
    struct Config { using Scalar = TF; enum { nb_dims = nd, use_AABB_bounds_on_cells = 0 }; };
    using Pt = Vec<TF,nd>;

    // load points
    std::ifstream f( filename );
    Vec<Pt> positions;
    Vec<TF> weights;
    if ( filename.ends_with( ".xyz32.bin" ) ) {
        for( PI i = 0; i < ( 1 << 22 ); ++i ) {
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
        for( PI i = 0; ; ++i ) {
            double x, y, z;
            f >> x >> y >> z;
            if ( ! f )
                break;
            positions << Pt{ x, y, z };
            weights << 0;
        }
    }
    // P( positions.size() );

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
    PointTreeCtorParms cp{ .max_nb_points = mp };
    PowerDiagram<Config> pd( cp, std::move( positions ), std::move( weights ), bnd_dirs, bnd_offs );
    // prof->toc( "setup" );

    // prof->tic( "cell" );

    auto t0 = std::chrono::steady_clock::now();

    Vec<TF> nv1( FromSizeAndItemValue(), pd.nb_cells() );
    Vec<TF> nc1( FromSizeAndItemValue(), pd.nb_cells() );
    Vec<TF> nv2( FromSizeAndItemValue(), pd.nb_cells() );
    Vec<TF> nc2( FromSizeAndItemValue(), pd.nb_cells() );
    Vec<TF> volumes( FromSizeAndItemValue(), pd.max_nb_threads(), 0 );
    Vec<typename PowerDiagram<Config>::CutInfo> prev_cuts( FromSize(), pd.nb_cells() );
    pd.for_each_cell( [&]( Cell<Config> &cell, int num_thread ) {
        nv1[ cell.i0 ] = cell.capa_vertices();
        nc1[ cell.i0 ] = cell.capa_cuts();

        cell.memory_compaction();

        volumes[ num_thread ] += cell.measure();

        nv2[ cell.i0 ] = cell.capa_vertices();
        nc2[ cell.i0 ] = cell.capa_cuts();

        for( const CellCut<Config> &cut : cell.cuts )
            if ( cut.type == CutType::Dirac )
                prev_cuts[ cell.i0 ][ cut.ptr ] << cut.num_in_ptr;
    } );

    P( mean( nv1 ) );
    P( mean( nv2 ) );
    P( mean( nc1 ) );
    P( mean( nc2 ) );

    auto t1 = std::chrono::steady_clock::now();
    std::cout << std::chrono::nanoseconds( t1 - t0 ).count() / 1e6 << "ms vol:" << sum( volumes ) << std::endl;

    for ( auto &v : volumes )
        v = 0;

    pd.for_each_cell( [&]( Cell<Config> &cell, int num_thread ) {
        nv1[ cell.i0 ] = cell.capa_vertices();
        nc1[ cell.i0 ] = cell.capa_cuts();

        cell.memory_compaction();

        volumes[ num_thread ] += cell.measure();

        nv2[ cell.i0 ] = cell.capa_vertices();
        nc2[ cell.i0 ] = cell.capa_cuts();
    }, prev_cuts.data() );

    auto t2 = std::chrono::steady_clock::now();
    std::cout << std::chrono::nanoseconds( t2 - t1 ).count() / 1e6 << "ms vol:" << sum( volumes ) << std::endl;

    // std::cout << *prof << std::endl;
    // P( mp, sum( volumes ) );
    P( mean( nv1 ) );
    P( mean( nv2 ) );
    P( mean( nc1 ) );
    P( mean( nc2 ) );
}


TEST_CASE( "Astro 3D", "" ) {
    //prof = new amgcl::profiler<>( "sdot" );
    // test_astro<double,3>( "/home/leclerc/test_amg/16M.xyz32.bin" );
    for( PI i = 18; i <= 18; i += 1 )
        test_astro<double,3>( "Points_1000000.xyz", i );
}
