#include "PowerDiagram/Cell.h"
#include "catch_main.h"
#include <cstdlib>

TEST_CASE( "Cell", "" ) {
    constexpr PI nb_dims = 2;
    using Scalar = double;

    using Point = Vec<Scalar,nb_dims>;

    Cell<Scalar,nb_dims> cell;
    cell.init_geometry_to_encompass( { FromItemValue(), -2 }, { FromItemValue(), 2 } );
    cell.p0 = { 0, 0 };
    cell.w0 = 0;

    auto r = []() { return Scalar( rand() ) / RAND_MAX; };

    for( PI i = 0; i < 11; ++i ) {
        Scalar a = r() * 2 * M_PI;
        cell.cut_boundary( { cos( a ), sin( a ) }, 1.0, i );
    }

    P( cell );
    cell.for_each_vertex( [](const auto &v ) { 
        P( v.num_cuts );
    } );

    VtkOutput vo;
    cell.display_vtk( vo );
    vo.save( "out.vtk" );
}
