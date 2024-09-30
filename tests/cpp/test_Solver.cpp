#include <tl/support/containers/operators/argmin.h>
#include <tl/support/containers/operators/mean.h>
#include <tl/support/string/va_string.h>
#include <boost/math/differentiation/autodiff.hpp>
#include "PowerDiagram/PowerDiagram.h"
#include "catch_main.h"

#include "matplotlibcpp.h"

template<class Scalar,int nb_dims>
class SdotSolver {
public:
    using Pd = PowerDiagram<Scalar,nb_dims>;
    using Point = Pd::Point;

    /**/ SdotSolver() {}

    void for_each_cell( const std::function<void( const Cell<Scalar,nb_dims> &cell, int )> &f, Span<Scalar> weights ) {
        PowerDiagram<Scalar,nb_dims> pd( PointTreeCtorParms{}, Vec<Point>( positions ), Vec<Scalar>( weights ), bnd_dirs, bnd_offs );
        pd.for_each_cell( f );
    }

    PI nb_cells() const {
        return positions.size();
    }

    Vec<Scalar> jacobi_dir( Span<Scalar> weights ) {
        Vec<Scalar> res( FromSize(), nb_cells() );
        for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell, int ) {
            using namespace boost::math::differentiation;
            using AD = autodiff_fvar<Scalar,1>;

            auto m = cell.measure( [&]( Scalar w, CutType type, PI i ) -> AD {
                AD res = w;
                if ( type == CutType::Dirac && i == cell.i0 )
                    res += make_fvar<Scalar, 1>( 0 );
                return res;
            } );

            const Scalar target_measure = Scalar( 1 ) / nb_cells();
            res[ cell.i0 ] = ( target_measure - m.derivative( 0 ) ) / m.derivative( 1 );
        }, weights );
        return res;
    }

    auto der_err( Span<Scalar> weights, Span<Scalar> dir ) {
        using namespace boost::math::differentiation;
        using AD = autodiff_fvar<Scalar,2>;
        using namespace std;

        Vec<AD> res( FromSizeAndItemValue(), Pd::max_nb_threads(), 0 );
        for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell, int num_thread ) {
            auto m = cell.measure( [&]( Scalar w, CutType type, PI i ) -> AD {
                if ( type == CutType::Dirac )
                    P( w, weights[ i ] );
                AD res = w;
                if ( type == CutType::Dirac )
                    res += make_fvar<Scalar,2>( 0 ) * dir[ i ];
                return res;
            } );

            const Scalar target_measure = Scalar( 1 ) / nb_cells();
            res[ num_thread ] += pow( m - target_measure, 2 );
        }, weights );

        return sum( res );
    }

    Vec<Scalar> measures( Span<Scalar> weights ) {
        Vec<Scalar> res( FromSizeAndItemValue(), nb_cells(), 17 );
        for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell, int num_thread ) {
            res[ cell.i0 ] = cell.measure();
        }, weights );
        return res;
    }

    Scalar error( Span<Scalar> weights ) {
        using std::pow;

        Vec<Scalar> errors( FromSizeAndItemValue(), PowerDiagram<Scalar,nb_dims>::max_nb_threads(), 0 );
        for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell, int num_thread ) {
            const Scalar target_measure = Scalar( 1 ) / nb_cells();
            errors[ num_thread ] += pow( cell.measure() - target_measure, 2 );
        }, weights );

        return sum( errors );
    }

    void display_vtk( VtkOutput &vo, Span<Scalar> weights ) {
        std::mutex m;
        for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell, int ) {
            m.lock();
            cell.display_vtk( vo );
            m.unlock();
        }, weights );
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

    Vec<Scalar> weights( FromSizeAndItemValue(), solver.nb_cells(), 0.0 );

    // Vec<Scalar> xs, ms;
    // auto mb = solver.measures( weights )[ solver.indices[ 0 ] ];
    // for( PI i = 0; i < 20; ++i ) {
    //     weights[ 0 ] = 0.5 + 1e-3 * i;
    //     xs << weights[ 0 ];
    //     ms << solver.measures( weights )[ solver.indices[ 0 ] ] - mb;

    //     VtkOutput vo;
    //     solver.display_vtk( vo, weights );
    //     vo.save( va_string( "out_$0.vtk", i ) );
    // }
    // P( mb );
    // P( xs );
    // P( ms );

    std::vector<Scalar> xs;
    std::vector<Scalar> es;
    std::vector<Scalar> vs;
    for( int iter = 0; iter < 2; ++iter ) {
        Vec<Scalar> dir = solver.jacobi_dir( weights );

        auto err = solver.der_err( weights, dir );

        //P( err, err.derivative( 1 ) / err.derivative( 2 ) );
        for( PI i = 0; i < 200; ++i ) {
            Scalar x = Scalar( i ) / 1000;
            xs.push_back( x );
            es.push_back( err.derivative( 0 ) + err.derivative( 1 ) * x + err.derivative( 2 ) * pow( x, 2 ) / 2 );
            vs.push_back( solver.error( weights + x * dir ) );
        }
        // break;

        Scalar alpha = err.derivative( 1 ) / err.derivative( 2 ) / 3;
        weights = weights - alpha * dir;
    }

    matplotlibcpp::plot( xs, es );
    matplotlibcpp::plot( xs, vs );
    matplotlibcpp::show();

    if ( filename.size() ) {
        VtkOutput vo;
        solver.display_vtk( vo, weights );
        vo.save( filename );
    }
}


TEST_CASE( "PowerDiagram 2D", "" ) {
    test_solver<double,3>( 30, "out.vtk" );

    // Vec<Vec<double,3>,3> M;
    // Vec<double,3> V;
    // for( auto &a : M )
    //     for( auto &v : a )
    //         v = rand();
    // for( auto &v : V )
    //     v = rand();
    // P( M, determinant( M ) );
    // P( M, V, lu_solve( M, V ) );
}
