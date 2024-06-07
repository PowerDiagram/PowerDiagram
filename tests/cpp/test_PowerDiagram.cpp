#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

TEST_CASE( "PowerDiagram", "" ) {
    Vec<Scalar> weights; // { 1, 1 };
    Vec<Point> points; // { { 0.25, 0.5 }, { 0.75, 0.5 } };
    Vec<PI> indices; // { 0, 1 };
    for( PI i = 0; i < 18; ++i ) {
        points << Point{ Scalar( rand() ) / RAND_MAX, Scalar( rand() ) / RAND_MAX };
        weights << 1;
        indices << i;
    }

    Vec<Scalar> bnd_offs{ 1, 1 };
    Vec<Point> bnd_dirs{ { +1, 0 }, { 0, +1 } };

    PointTreeCtorParms cp{ .max_nb_points = 20 };
    PowerDiagram pd( cp, points, weights, indices, bnd_dirs, bnd_offs );

    VtkOutput vo;
    pd.for_each_cell( [&]( const Cell &cell ) {
        cell.display_vtk( vo );
        // P( cell );
    } );

    vo.save( "pd.vtk" );
}
