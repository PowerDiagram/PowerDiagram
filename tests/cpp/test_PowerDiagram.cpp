#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

TEST_CASE( "PowerDiagram", "" ) {
    Vec<Point> points{ { 0.25, 0.5 }, { 0.75, 0.5 } };
    Vec<Scalar> weights{ 1, 1 };
    Vec<PI> indices{ 0, 1 };

    PointTreeCtorParms cp{ .max_nb_points = 20 };
    PowerDiagram pd( cp, points, weights, indices );
    P( pd );
}
