#include <string>
#include <tl/support/string/va_string.h>
#include <tl/support/operators/argmin.h>
#include <tl/support/operators/mean.h>
#include "SdotSolver.h"
#include "catch_main.h"

#include <matplotlibcpp.h>
#include <vector>

constexpr PI nd = SdotSolver::Config::nb_dims;
using TF = SdotSolver::TF;
using Pt = SdotSolver::Pt;

Vec<TF> solve_smooth( SdotSolver &solver, PI nb_additionnal_dirs, TF nb_smooth_per_dir ) {
    Vec<TF> weights( FromSizeAndItemValue(), solver.nb_cells(), 0.0 );
    P( nb_additionnal_dirs, nb_smooth_per_dir );

    Vec<TF> errs;
    Vec<Vec<TF>> dirs( FromReservationSize(), 1 + nb_additionnal_dirs );
    for( int iter = 0; iter < 10; ++iter ) {
        // while ( dirs.size() > 1 + nb_additionnal_dirs )
        //     dirs.remove( 0, 1 );
        // if ( dirs.size() )
        //     dirs.resize( 1 );
        dirs.clear();

        dirs << solver.jacobi_dir( weights );
        for( PI ba = 0; ba < nb_additionnal_dirs; ++ba )
            dirs << solver.smooth( weights, dirs.back(), int( pow( nb_smooth_per_dir, ba ) ) );

        auto err = solver.der_err( weights, dirs );
        errs.push_back( err.S );
        P( err.S * 1e9 );

        if ( errs.size() >= 9 )
            return errs;

        auto alpha = err.argmin();
        TF relax = 1.0;
        for( PI d = 0; d < 20; ++d, relax /= 2 ) {
            Vec<TF> new_weights;
            if ( alpha.size() == 1 ) {
                new_weights = weights;
                for( PI i = 0; i < alpha.size(); ++i )
                    new_weights = new_weights + relax * min( alpha[ i ], 0.8 * err.max_X[ i ] ) * dirs[ i ];
            } else {
                Vec<TF> new_dir = alpha[ 0 ] * dirs[ 0 ];
                for( PI n = 1; n < dirs.size(); ++n )
                    new_dir = new_dir + alpha[ n ] * dirs[ n ];

                TF corr = min( TF( 1 ), 0.9 * solver.max_alpha_for( weights, new_dir ) );
                new_weights = weights + relax * corr * new_dir;
            }

            bool void_cell = false;
            solver.error( new_weights, &void_cell );
            if ( ! void_cell ) {
                weights = std::move( new_weights );
                break;
            }
            P( "nim" );
        }
    }

    errs.push_back( solver.error( weights ) );

    return errs;
}

Vec<TF> solve_smooth( SdotSolver &solver, PI nb_additionnal_dirs ) {
    Vec<TF> res = solve_smooth( solver, nb_additionnal_dirs, 0 );
    if ( nb_additionnal_dirs == 0 )
        return res;

    PI best_nb_smooth = 0;
    for( PI i = 1; i < 11; ++i ) {
        Vec<TF> loc = solve_smooth( solver, nb_additionnal_dirs, i );
        if ( res.back() > loc.back() ) {
            best_nb_smooth = i;
            res = loc;
        }
    }
    P( best_nb_smooth );

    return res;
}

Vec<TF> solve_newton( SdotSolver &solver ) {
    Vec<TF> target_masses( FromSizeAndItemValue(), solver.nb_cells(), TF( 1 ) / solver.nb_cells() );
    Vec<TF> weights( FromSizeAndItemValue(), solver.nb_cells(), 0.0 );

    Vec<TF> errs;
    for( int iter = 0; ; ++iter ) {
        auto vec = solver.measures( weights ) - target_masses;
        auto sys = solver.matrix( weights );
        auto sol = solve( sys, vec );

        for( TF coeff = 1.0; ; coeff /= 2 ) {
            auto prop = weights - coeff * sol;

            bool void_cell = false;
            TF err = solver.error( prop, &void_cell );

            if ( ! void_cell ) {
                weights = std::move( prop );

                errs.push_back( err );
                if ( errs.size() >= 9 )
                    return errs;
                
                break;
            }
        }
    }


    return errs;
}

void plot_err( Str name, Span<TF> err ) {
    std::vector<TF> num_iters;
    for( PI i = 0; i < err.size(); ++i )
        num_iters.push_back( i );
    matplotlibcpp::named_semilogy( name.c_str(), num_iters, std::vector<TF>( err.begin(), err.end() ) );
}

void test_solver( PI nb_cells, std::string filename = {} ) {
    SdotSolver solver;

    // diracs
    for( PI i = 0; i < nb_cells; ++i ) {
        Pt p;
        for( PI d = 0; d < nd; ++d )
            p[ d ] = TF( rand() ) / RAND_MAX * ( d ? 1.0 : 0.5 );
        solver.positions << p;
        solver.indices << i;
    }

    // boundaries
    for( PI d = 0; d < nd; ++d ) {
        Pt p( FromItemValue(), 0 ); p[ d ] = +1;
        Pt q( FromItemValue(), 0 ); q[ d ] = -1;
        solver.bnd_offs << 1 << 0;
        solver.bnd_dirs << p << q;
    }

    plot_err( "newton", solve_newton( solver ) );
    // plot_err( "jaco_0", solve_smooth( solver, 0 ) );
    // plot_err( "jaco_1", solve_smooth( solver, 1, 23 ) );
    // plot_err( "jaco_2", solve_smooth( solver, 2, 15 ) );
    // plot_err( "jaco_3", solve_smooth( solver, 3, 11 ) );
    // plot_err( "jaco_4", solve_smooth( solver, 4,  9 ) );
    // 0.13
    plot_err( "jaco_5_e4.1", solve_smooth( solver, 5, 4.1 ) );
    // plot_err( "jaco_5_e4.1", solve_smooth( solver, 5, 4.1 ) );
    // plot_err( "jaco_9", solve_smooth( solver, 9, 10 ) );
    // for( PI i = 6; i < 10; ++i )
        // plot_err( "jaco_" + std::to_string( i ), solve_smooth( solver, i ) );
    matplotlibcpp::legend();
    matplotlibcpp::show();

    // P( solver.matrix( weights ) );

    // if ( filename.size() ) {
    //     VtkOutput vo;
    //     solver.display_vtk( vo, weights );
    //     vo.save( filename );
    // }
}


TEST_CASE( "PowerDiagram 3D", "" ) {
    test_solver( 10000, "out.vtk" );
}
