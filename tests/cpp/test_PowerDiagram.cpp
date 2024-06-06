#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

TEST_CASE( "PowerDiagram", "" ) {
    Vec<Point> points; // { { 0.25, 0.5 }, { 0.75, 0.5 } };
    Vec<Scalar> weights; // { 1, 1 };
    Vec<PI> indices; // { 0, 1 };
    for( PI i = 0; i < 18; ++i ) {
        points << Point{ Scalar( rand() ) / RAND_MAX, Scalar( rand() ) / RAND_MAX };
        weights << 1;
        indices << i;
    }

    PointTreeCtorParms cp{ .max_nb_points = 20 };
    PowerDiagram pd( cp, points, weights, indices );

    VtkOutput vo;
    pd.for_each_cell( [&]( const Cell &cell ) {
        cell.display_vtk( vo );
        // P( cell );
    } );

    vo.save( "pd.vtk" );
}
