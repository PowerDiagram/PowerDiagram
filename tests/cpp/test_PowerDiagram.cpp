#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

template<class Scalar,int nb_dims>
void test_rand( PI nb_cells, PI max_nb_points, std::string filename = {} ) {
    using Point = Vec<Scalar,nb_dims>;

    Vec<Scalar> weights;
    Vec<Point> points;
    Vec<PI> indices;
    for( PI i = 0; i < nb_cells; ++i ) {
        Point p;
        for( PI d = 0; d < nb_dims; ++d )
            p[ d ] = Scalar( rand() ) / RAND_MAX;
        weights << 0;
        indices << i;
        points  << p;
    }

    Vec<Scalar> bnd_offs;
    Vec<Point> bnd_dirs;
    for( PI d = 0; d < nb_dims; ++d ) {
        Point p( FromItemValue(), 0 ); p[ d ] = +1;
        Point q( FromItemValue(), 0 ); q[ d ] = -1;
        bnd_offs << 1 << 0;
        bnd_dirs << p << q;
    }

    auto tStartSteady = std::chrono::steady_clock::now();

    PointTreeCtorParms cp{ .max_nb_points = max_nb_points };
    PowerDiagram<Scalar,nb_dims> pd( cp, points, weights, indices, bnd_dirs, bnd_offs );

    Scalar volume = 0;
    pd.for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell ) {
        volume += cell.measure();
    } );
    auto tEndSteady = std::chrono::steady_clock::now();
    std::chrono::nanoseconds diff = tEndSteady - tStartSteady;
    std::cout << "Time taken = " << diff.count() / 1e6 << " ms; volume = " << volume << " for " << max_nb_points << std::endl;

    if ( filename.size() ) {
        VtkOutput vo;
        pd.for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell ) {
            cell.display_vtk( vo );
        } );
        vo.save( "pd.vtk" );
    }
}

TEST_CASE( "PowerDiagram 2D", "" ) {
    // 2D => 20
    for( PI n = 4; n < 50; ++n )
        test_rand<double,4>( 500, n );
}
