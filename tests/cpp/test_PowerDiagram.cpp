#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

TEST_CASE( "PowerDiagram", "" ) {
    constexpr int nb_dims = 2;
    using Scalar = double;

    using Point = Vec<Scalar,nb_dims>;

    Vec<Scalar> weights; // { 1, 1 };
    Vec<Point> points; // { { 0.25, 0.5 }, { 0.75, 0.5 } };
    Vec<PI> indices; // { 0, 1 };
    for( PI i = 0; i < 5000; ++i ) {
        points << Point{ Scalar( rand() ) / RAND_MAX, Scalar( rand() ) / RAND_MAX };
        weights << points.back()[ 1 ] * 0.3 + 0.2;
        indices << i;
    }

    Vec<Scalar> bnd_offs{ 1, 1, 0, 0 };
    Vec<Point> bnd_dirs{ { +1, 0 }, { 0, +1 }, { -1, 0 }, { 0, -1 } };

    PointTreeCtorParms cp{ .max_nb_points = 4 };
    PowerDiagram<Scalar,nb_dims> pd( cp, points, weights, indices, bnd_dirs, bnd_offs );

    VtkOutput vo;
    Scalar volume = 0;
    pd.for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell ) {
        volume += cell.measure();
        cell.display_vtk( vo );
    } );
    vo.save( "pd.vtk" );

    P( volume );
}
