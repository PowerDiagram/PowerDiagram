#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

void test( double shrink, std::string name, const Vec<double> &values ) {
    constexpr int nb_dims = 2;
    using Scalar = double;

    using Point = Vec<Scalar,nb_dims>;

    Vec<Scalar> weights; // { 1, 1 };
    Vec<Point> points; // { { 0.25, 0.5 }, { 0.75, 0.5 } };
    Vec<PI> indices; // { 0, 1 };
    for( PI i = 0, j = 0; i < 20; ++i ) {
        points << Point{ shrink * values[ j++ ], values[ j++ ] };
        weights << 1;
        indices << i;
    }

    Vec<Scalar> bnd_offs{ shrink, 1, 0, 0 };
    Vec<Point> bnd_dirs{ { +1, 0 }, { 0, +1 }, { -1, 0 }, { 0, -1 } };

    PointTreeCtorParms cp{ .max_nb_points = 200 };
    PowerDiagram<Scalar,nb_dims> pd( cp, points, weights, indices, bnd_dirs, bnd_offs );

    VtkOutput vo;
    pd.for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell ) {
        cell.display_vtk( vo );
    } );
    vo.save( name + ".vtk" );
}

TEST_CASE( "PowerDiagram shrink", "" ) {
    Vec<double> values;
    for( PI i = 0; i < 40; ++i )
        values << double( rand() ) / RAND_MAX;    
    
    test( 1.0, "pa", values );
    test( 0.8, "pb", values );
}
