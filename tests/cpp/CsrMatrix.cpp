#include "CsrMatrix.h"

#include <amgcl/backend/builtin.hpp>
#include <amgcl/adapter/crs_tuple.hpp>
#include <amgcl/make_solver.hpp>
#include <amgcl/amg.hpp>
#include <amgcl/coarsening/smoothed_aggregation.hpp>
#include <amgcl/relaxation/spai0.hpp>
#include <amgcl/solver/bicgstab.hpp>

#include <amgcl/io/mm.hpp>
#include <cstddef>
#include <vector>

Vec<double> solve( const CsrMatrix<double> &sys, Span<double> vec ) {
    typedef amgcl::backend::builtin<double> SBackend;
    typedef amgcl::backend::builtin<double> PBackend;
    typedef amgcl::make_solver<
        amgcl::amg<
            PBackend,
            amgcl::coarsening::smoothed_aggregation,
            amgcl::relaxation::spai0
        >,
        amgcl::solver::bicgstab<SBackend>
    > Solver;

    std::vector<ptrdiff_t> offsets( sys.offsets.begin(), sys.offsets.end() );
    std::vector<ptrdiff_t> columns( sys.columns.begin(), sys.columns.end() );
    std::vector<double> values( sys.values.begin(), sys.values.end() );
    std::vector<double> V( vec.begin(), vec.end() );
    values[ 0 ] += 1e4;

    ptrdiff_t rows = sys.nb_rows();
    auto A = std::tie( rows, offsets, columns, values );

    Solver::params prm;
    prm.solver.tol = 1e-10;

    // Initialize the solver with the system matrix:
    Solver solve( A, prm );

    // Show the mini-report on the constructed solver:
    // std::cout << solve << std::endl;

    // Solve the system with the zero initial approximation:
    int iters;
    double error;
    std::vector<double> x( rows, 0.0 );

    std::tie( iters, error ) = solve( A, V, x );

    // Output the number of iterations, the relative error,
    // and the profiling data:
    // std::cout << "Iters: " << iters << std::endl
    //           << "Error: " << error << std::endl;
    //           << prof << std::endl;

    return x;
}
