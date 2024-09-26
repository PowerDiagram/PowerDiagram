#include <tl/support/containers/operators/mean.h>
#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

template<class Scalar,int nb_dims>
class SdotSolver {
public:
    using       Point     = PowerDiagram<Scalar,nb_dims>::Point;

    /**/        SdotSolver() {}

    void for_each_cell( const std::function<void( const Cell<Scalar,nb_dims> &cell, int )> &f, Span<Scalar> weights ) {
        PowerDiagram<Scalar,nb_dims> pd( PointTreeCtorParms{}, positions, weights, indices, bnd_dirs, bnd_offs );
        pd.for_each_cell( f );
    }

    PI nb_cells() const {
        return positions.size();
    }

    Vec<Scalar> jacobi_dir( Span<Scalar> weights ) {
        Vec<Scalar> res( FromSize(), nb_cells() );
        for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell, int ) {
            const Scalar target_measure = Scalar( 1 ) / nb_cells();
            res[ cell.orig_index ] = cell.measure() - target_measure;
        }, weights );
        return res;
    }

    Scalar      error    ( Span<Scalar> weights ) {
        using std::pow;

        Vec<Scalar> errors( FromSizeAndItemValue(), PowerDiagram<Scalar,nb_dims>::max_nb_threads(), 0 );
        for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell, int num_thread ) {
            const Scalar target_measure = Scalar( 1 ) / nb_cells();
            errors[ num_thread ] += pow( cell.measure() - target_measure, 2 );
        }, weights );
        return sum( errors );
    }

    Vec<Point>  positions;
    Vec<PI>     indices;

    Vec<Scalar> bnd_offs;
    Vec<Point>  bnd_dirs;
};

template<class Scalar,int nb_dims>
void test_solver( PI nb_cells, std::string filename = {} ) {
    using Point = Vec<Scalar,nb_dims>;

    SdotSolver<Scalar,nb_dims> solver;

    for( PI i = 0; i < nb_cells; ++i ) {
        Point p;
        for( PI d = 0; d < nb_dims; ++d )
            p[ d ] = Scalar( rand() ) / RAND_MAX;
        solver.positions << p;
        solver.indices << i;
    }

    for( PI d = 0; d < nb_dims; ++d ) {
        Point p( FromItemValue(), 0 ); p[ d ] = +1;
        Point q( FromItemValue(), 0 ); q[ d ] = -1;
        solver.bnd_offs << 1 << 0;
        solver.bnd_dirs << p << q;
    }

    Vec<Scalar> weights( FromSizeAndItemValue(), solver.nb_cells(), 0 );

    Vec<Scalar> dir = solver.jacobi_dir( weights );
    for( int i = -10; i < 10; ++i ) {
        Vec<Scalar> prop = weights + i / Scalar( 40 ) * dir;
        P( i, solver.error( prop ) );
    }

    // if ( filename.size() ) {
    //     VtkOutput vo;
    //     pd.display_vtk( vo );
    //     vo.save( filename );
    // }
}


TEST_CASE( "PowerDiagram 2D", "" ) {
    test_solver<double,2>( 50, "out.vtk" );
}
