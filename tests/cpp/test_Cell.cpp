#include "PowerDiagram/Cell.h"
#include "catch_main.h"
#include <cstdlib>

// TEST_CASE( "Cell", "" ) {
//     constexpr PI nb_dims = 2;
//     using Scalar = double;

//     using Point = Vec<Scalar,nb_dims>;

//     Cell<Scalar,nb_dims> cell;
//     cell.init_geometry_to_encompass( { FromItemValue(), -2 }, { FromItemValue(), 2 } );
//     cell.p0 = { 0, 0 };
//     cell.w0 = 0;
//     cell.i0 = 0;

//     auto r = []() { return Scalar( rand() ) / RAND_MAX; };

//     // for( PI i = 0; i < 3; ++i ) {
//     //     Scalar a = r() * 2 * M_PI;
//     //     cell.cut_boundary( { cos( a ), sin( a ) }, 1.0, i );
//     // }

//     P( cell, cell.measure() );
//     // cell.memory_compaction();
//     // P( cell, cell.measure() );
//     // cell.for_each_vertex( [](const auto &v ) { 
//     //     P( v.num_cuts );
//     // } );

//     VtkOutput vo;
//     cell.display_vtk( vo );
//     vo.save( "out.vtk" );
// }

// void test_3d( bool compaction ) {
//     constexpr PI nb_dims = 3;
//     using Scalar = double;

//     using Point = Vec<Scalar,nb_dims>;

//     Cell<Scalar,nb_dims> cell;
//     cell.init_geometry_to_encompass( { FromItemValue(), -2 }, { FromItemValue(), 2 } );
//     cell.p0 = { 0, 0 };
//     cell.w0 = 0;
//     cell.i0 = 0;

//     for( PI d = 0; d < nb_dims; ++d ) {
//         Point p( FromItemValue(), 0 ); p[ d ] = +1;
//         Point q( FromItemValue(), 0 ); q[ d ] = -1;
//         cell.cut_boundary( p, 1, 2 * d + 0 );
//         cell.cut_boundary( q, 0, 2 * d + 1 );
//     }

//     //P( cell, cell.measure() );
//     if ( compaction )
//         cell.memory_compaction();
//     P( cell );

//     cell.cut_boundary( { 0.5, 0.5, 0.5 }, 0.15, 1 );

//     // if ( compaction )
//     //     cell.memory_compaction();
//     P( cell );
//     P( cell.measure() );

//     cell.for_each_edge( [&]( auto c, auto v ) {
//         P( c, v );
//     } );

//     // VtkOutput vo;
//     // cell.display_vtk( vo );
//     // vo.save( "out.vtk" );
// }

// TEST_CASE( "Cell 3D", "" ) {
// }

TEST_CASE( "Cell 2D", "" ) {
    struct Config { using Scalar = double; enum { nb_dims = 2, use_AABB_bounds_on_cells = 0 }; };
    using Pt = Vec<double,2>;

    Cell<Config> cell;
    cell.init_geometry_to_encompass( { FromItemValue(), -2 }, { FromItemValue(), 2 } );
    cell.p0 = { 0, 0 };
    cell.w0 = 0;
    cell.i0 = 0;

    // for( PI d = 0; d < nb_dims; ++d ) {
    //     Point p( FromItemValue(), 0 ); p[ d ] = +1;
    //     Point q( FromItemValue(), 0 ); q[ d ] = -1;
    //     cell.cut_boundary( p, 1, 2 * d + 0 );
    //     cell.cut_boundary( q, 0, 2 * d + 1 );
    // }

    cell.cut_boundary( { -1, 0 }, 0, 1 );
    cell.cut_boundary( { 0, -1 }, 0, 1 );
    cell.cut_boundary( { 2, 1 }, 1, 1 );
    cell.memory_compaction();
    P( cell );

    // if ( compaction )
    //     cell.memory_compaction();
    P( cell.for_each_cut_with_measure( [&]( const CellCut<Config> &cut, double measure ) {
        P( cut, measure );
    } ) );

    P( cell.measure() );

    // VtkOutput vo;
    // cell.display_vtk( vo );
    // vo.save( "out.vtk" );
}
