#include <tl/support/operators/mean.h>
#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"
#include <chrono>

// template<class Scalar,int nb_dims>
// void test_rand( PI nb_cells, PI max_nb_points, std::string filename = {} ) {
//     using Point = Vec<Scalar,nb_dims>;

//     Vec<Scalar> weights;
//     Vec<Point> points;
//     Vec<PI> indices;
//     for( PI i = 0; i < nb_cells; ++i ) {
//         Point p;
//         for( PI d = 0; d < nb_dims; ++d )
//             p[ d ] = Scalar( rand() ) / RAND_MAX;
//         weights << 0;
//         indices << i;
//         points  << p;
//     }

//     Vec<Scalar> bnd_offs;
//     Vec<Point> bnd_dirs;
//     for( PI d = 0; d < nb_dims; ++d ) {
//         Point p( FromItemValue(), 0 ); p[ d ] = +1;
//         Point q( FromItemValue(), 0 ); q[ d ] = -1;
//         bnd_offs << 1 << 0;
//         bnd_dirs << p << q;
//     }

//     auto tStartSteady = std::chrono::steady_clock::now();

//     PointTreeCtorParms cp{ .max_nb_points = max_nb_points };
//     PowerDiagram<Scalar,nb_dims> pd( cp, points, weights, indices, bnd_dirs, bnd_offs );

//     Scalar volume = 0;
//     pd.for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell ) {
//         volume += cell.measure();
//     } );
//     auto tEndSteady = std::chrono::steady_clock::now();
//     std::chrono::nanoseconds diff = tEndSteady - tStartSteady;
//     std::cout << "Time taken = " << diff.count() / 1e6 << " ms; volume = " << volume << " for " << max_nb_points << std::endl;

//     if ( filename.size() ) {
//         VtkOutput vo;
//         pd.for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell ) {
//             cell.display_vtk( vo );
//         } );
//         vo.save( "pd.vtk" );
//     }
// }

// TEST_CASE( "PowerDiagram 2D", "" ) {
//     // 2D => 20
//     for( PI n = 4; n < 50; ++n )
//         test_rand<double,3>( 500, n );
// }

template<class TF,int nd>
void test_speed( PI nb_cells, std::string filename = {} ) {
    struct Config { using Scalar = TF; enum { nb_dims = nd, use_AABB_bounds_on_cells = 0 }; };
    using Point = Vec<TF,nd>;

    Vec<TF> weights;
    Vec<Point> points;
    for( PI i = 0; i < nb_cells; ++i ) {
        Point p;
        for( PI d = 0; d < nd; ++d )
            p[ d ] = TF( rand() ) / RAND_MAX;
        weights << 0;
        points  << p;
    }

    Vec<TF> bnd_offs;
    Vec<Point> bnd_dirs;
    for( PI d = 0; d < nd; ++d ) {
        Point p( FromItemValue(), 0 ); p[ d ] = +1;
        Point q( FromItemValue(), 0 ); q[ d ] = -1;
        bnd_offs << 1 << 0;
        bnd_dirs << p << q;
    }

    auto t0 = std::chrono::steady_clock::now();

    PointTreeCtorParms cp;
    PowerDiagram<Config> pd( cp, std::move( points ), std::move( weights ), bnd_dirs, bnd_offs );

    Vec<TF> v0( FromSizeAndItemValue(), pd.max_nb_threads(), 0 );
    Vec<TF> v1( FromSizeAndItemValue(), pd.max_nb_threads(), 0 );
    Vec<TF> nv0( FromSize(), pd.nb_cells() ), nv1( FromSize(), pd.nb_cells() ); // 22 en moyenne
    Vec<TF> nc0( FromSize(), pd.nb_cells() ), nc1( FromSize(), pd.nb_cells() ); // 22 en moyenne
    Vec<PrevCutInfo<Config>> prev_cuts( FromSize(), pd.nb_cells() );
    pd.for_each_cell( [&]( Cell<Config> &cell, int num_thread ) {
        nv0[ cell.i0 ] = cell.capa_vertices();
        nc0[ cell.i0 ] = cell.capa_cuts();

        // v0[ num_thread ] += cell.measure();
        cell.memory_compaction();

        nv1[ cell.i0 ] = cell.capa_vertices();
        nc1[ cell.i0 ] = cell.capa_cuts();

        cell.get_prev_cut_info( prev_cuts[ cell.i0 ] );
    } );

    auto t1 = std::chrono::steady_clock::now();
    std::cout << "Time taken = " << std::chrono::nanoseconds( t1 - t0 ).count() / 1e6 << " ms, volume = " << sum( v0 ) << std::endl;

    // Vec<TF> nboxes( FromSize(), pd.nb_cells() );
    // for( PI n = 0; n < pd.nb_cells(); ++n )
    //     nboxes[ n ] = prev_cuts[ n ].by_leaf.size();

    // P( mean( nboxes ) );
    // P( min( nboxes ) );
    // P( max( nboxes ) );

    P( mean( nv0 ) );
    P( mean( nv1 ) );
    P( mean( nc0 ) );
    P( mean( nc1 ) );

    pd.for_each_cell( [&]( Cell<Config> &cell, int num_thread ) {
        v1[ num_thread ] += cell.measure();
    }, prev_cuts.data() );

    auto t2 = std::chrono::steady_clock::now();
    std::cout << "Time taken = " << std::chrono::nanoseconds( t2 - t1 ).count() / 1e6 << " " << sum( v1 ) << std::endl;

    // P( mean( nv0 ) );
    // P( mean( nv1 ) );
    // P( mean( nc0 ) );
    // P( mean( nc1 ) );

    // if ( filename.size() ) {
    //     VtkOutput vo;
    //     pd.for_each_cell( [&]( const Cell<Config> &cell ) {
    //         cell.display_vtk( vo );
    //     } );
    //     vo.save( filename );
    // }
}


TEST_CASE( "PowerDiagram 3D", "" ) {
    // test_speed<double,3>( 3, "out.vtk" ); 4.5
    test_speed<double,3>( 500000, "out.vtk" );
}
