#include <sdot/Cell.h>
#include "catch_main.h"

TEST_CASE( "Cell", "" ) {
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
    cell.cut_boundary( { 1, 1 }, 1, 1 );
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
