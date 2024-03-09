#include <PowerDiagram/PowerDiagram.h>
#include <PowerDiagram/VtkOutput.h>
#include "catch_main.h"

TEST_CASE( "PowerDiagram", "" ) {
    List<Point> positions{ { 0.0, 0.0 }, { 1.0, 0.0 }, { 0.0, 1.0 } };
    Vector weights{ 0, 0, 0 };


    List<Point> bnd_vec;
    Vector bnd_off;
    for( PI i = 0, n = 5; i < n; ++i ) {
        FP64 a = i * 2 * M_PI / n;
        bnd_vec << Point{ cos( a ), sin( a ) };
        bnd_off << 1;
    }

    WeightedPointSet wps = make_WeightedPointSet_AABB( positions, weights );
    PowerDiagram pd( std::move( wps ) );

    pd.display_vtk( "test.vtk" );
}
