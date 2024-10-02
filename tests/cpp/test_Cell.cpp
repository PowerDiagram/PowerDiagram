#include "PowerDiagram/Cell.h"
#include "catch_main.h"

TEST_CASE( "Cell", "" ) {
    constexpr PI nb_dims = 2;
    using Scalar = double;

    using Point = Vec<Scalar,nb_dims>;

    Cell<Scalar,nb_dims> cell;
    cell.make_init_simplex( { FromItemValue(), 0 }, { FromItemValue(), 1 } );
    cell.p0 = { 0, 0 };
    cell.w0 = 0;

    cell.cut_boundary( { 1.0, 1.0 }, 0.5, 10 );

    // cell.cut_boundary( { 0.1, 1.0 }, 1.0, 11 );
    // P( cell );

    // VtkOutput vo;
    // cell.display_vtk( vo );
    // vo.save( "out.vtk" );
}
