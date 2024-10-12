#include <limits>
#include <tl/support/operators/norm_2.h>
#include <tl/support/operators/sum.h>

#include "../../src/cpp/PowerDiagram/PowerDiagram.h"
#include "SdotSolver.h"

#include <eigen3/Eigen/LU>

using Pd = PowerDiagram<SdotSolver::Config>;

void SdotSolver::for_each_cell( const std::function<void( const Cell<Config> &cell, int )> &f, Span<TF> weights ) {
    Pd pd( PointTreeCtorParms{}, Vec<Pt>( positions ), Vec<TF>( weights ), bnd_dirs, bnd_offs );
    pd.for_each_cell( f );
}

PI SdotSolver::nb_cells() const {
    return positions.size();
}

Vec<SdotSolver::TF> SdotSolver::jacobi_dir( Span<TF> weights ) {
    Vec<TF> res( FromSize(), nb_cells() );
    for_each_cell( [&]( const Cell<Config> &cell, int ) {
        TF sum = 0;
        TF m = cell.for_each_cut_with_measure( [&]( const CellCut<Config> &cut, TF measure ) {
            if ( cut.type != CutType::Dirac )
                return;
            TF der = measure / ( 2 * norm_2( cut.p1 - cell.p0 ) );
            sum += der;
        } );

        res[ cell.i0 ] = ( target_measure( cell.i0 ) - m ) / sum;
    }, weights );
    return res;
}

void SdotSolver::Ders::clear( PI nb_vars ) {
    max_X = { FromSizeAndItemValue(), nb_vars, std::numeric_limits<TF>::max() };

    M = { FromSizeAndItemValue(), nb_vars, FromSizeAndItemValue(), nb_vars, 0 };
    V = { FromSizeAndItemValue(), nb_vars, 0 };
    S = 0;
}

Vec<SdotSolver::TF> SdotSolver::Ders::argmin() const {
    using TM = Eigen::Matrix<TF,Eigen::Dynamic,Eigen::Dynamic>;
    using TV = Eigen::Matrix<TF,Eigen::Dynamic,1>;

    TM eM( M.size(), M.size() );
    TV eV( M.size() );
    for( PI r = 0; r < M.size(); ++r ) {
        for( PI c = 0; c < M.size(); ++c )
            eM.coeffRef( r, c ) = M[ r ][ c ];
        eV[ r ] = -0.5 * V[ r ];
    }

    Eigen::FullPivLU<TM> lu( eM );
    return lu.solve( eV );
}

SdotSolver::Ders SdotSolver::der_err_ap( Span<TF> w1, Span<TF> dir, TF eps ) {
    using std::pow;

    Vec<TF> w0( FromSize(), dir.size() ); 
    Vec<TF> w2( FromSize(), dir.size() ); 
    for( PI i = 0; i < dir.size(); ++i ) {
        w0[ i ] = w1[ i ] - eps * dir[ i ];
        w2[ i ] = w1[ i ] + eps * dir[ i ];
    }

    TF e0 = error( w0 );
    TF e1 = error( w1 );
    TF e2 = error( w2 );

    Ders res;
    res.M = { { ( ( e0 + e2 ) / 2 - e1 ) / pow( eps, 2 )} };
    res.V = { ( e2 - e1 ) / eps };
    res.S = e1;

    return res;
}

Vec<SdotSolver::TF> SdotSolver::smooth( Span<TF> weights, Span<TF> dir, PI rec ) {
    using std::abs;

    Vec<TF> res( FromSize(), positions.size() );
    for_each_cell( [&]( const Cell<Config> &cell, int ) {
        TF sum = 0, pnd = 0;
        TF m = cell.for_each_cut_with_measure( [&]( const CellCut<Config> &cut, TF measure ) {
            if ( cut.type != CutType::Dirac )
                return;
            TF der = measure / ( 2 * norm_2( cut.p1 - cell.p0 ) );
            sum += abs( der ) * dir[ cut.i1 ];
            pnd += abs( der );
        } );

        res[ cell.i0 ] = 0.5 * dir[ cell.i0 ] + 0.5 * sum / pnd;
    }, weights );

    if ( rec )
        return smooth( weights, res, rec - 1 );
    return res;
}

SdotSolver::Ders SdotSolver::der_err( Span<TF> weights, Span<Vec<TF>> dirs ) {
    using std::pow;
    using std::min;

    Vec<Ders> ders_for_each_thread( FromSizeAndItemValue(), Pd::max_nb_threads() );
    for( Ders &ders : ders_for_each_thread )
        ders.clear( dirs.size() );

    for_each_cell( [&]( const Cell<Config> &cell, int num_thread ) {
        auto &loc = ders_for_each_thread[ num_thread ];

        // err = ( d + alpha * sum( der * ( dir[ 0 ] - dir[ 1 ] ) ) ) ^ 2
        // err = d ^ 2 + alpha * d * sum( der * ( dir[ 0 ] - dir[ 1 ] ) ) + alpha^2 * sum( der * ( dir[ 0 ] - dir[ 1 ] ) )

        Vec<TF> sums( FromSizeAndItemValue(), dirs.size(), 0 );
        TF m = cell.for_each_cut_with_measure( [&]( const CellCut<Config> &cut, TF measure ) {
            if ( cut.type != CutType::Dirac )
                return;
            TF der = measure / ( 2 * norm_2( cut.p1 - cell.p0 ) );
            for( PI n = 0; n < dirs.size(); ++n )
                sums[ n ] += der * ( dirs[ n ][ cell.i0 ] - dirs[ n ][ cut.i1 ] );
        } );

        TF d = target_measure( cell.i0 ) - m;

        for( PI r = 0; r < dirs.size(); ++r ) {
            for( PI c = 0; c < dirs.size(); ++c )
                loc.M[ r ][ c ] += sums[ r ] * sums[ c ];
            loc.V[ r ] -= 2 * d * sums[ r ];
        }
        loc.S += pow( d, 2 );

        for( PI n = 0; n < dirs.size(); ++n )
            if ( sums[ n ] < 0 )
                loc.max_X[ n ] = min( loc.max_X[ n ], - m / sums[ n ] );
    }, weights );

    auto &res = ders_for_each_thread[ 0 ];
    for( PI i = 1; i < ders_for_each_thread.size(); ++i ) {
        const auto &loc = ders_for_each_thread[ i ];
        res.max_X = min( res.max_X, loc.max_X );
        res.M = res.M + loc.M;
        res.V = res.V + loc.V;
        res.S = res.S + loc.S;
    }

    return res;
}

Vec<SdotSolver::TF> SdotSolver::measures( Span<TF> weights ) {
    Vec<TF> res( FromSizeAndItemValue(), nb_cells() );
    for_each_cell( [&]( const Cell<Config> &cell, int num_thread ) {
        res[ cell.i0 ] = cell.measure();
    }, weights );
    return res;
}

SdotSolver::TF SdotSolver::error( Span<TF> weights, bool *void_cell ) {
    using std::pow;

    Vec<TF> errors( FromSizeAndItemValue(), Pd::max_nb_threads(), 0 );
    for_each_cell( [&]( const Cell<Config> &cell, int num_thread ) {
        const TF target_measure = TF( 1 ) / nb_cells();
        TF measure = cell.measure();
        if ( void_cell && measure == 0 )
            *void_cell = true;

        errors[ num_thread ] += pow( measure - target_measure, 2 );
    }, weights );

    return sum( errors );
}

void SdotSolver::display_vtk( VtkOutput &vo, Span<TF> weights ) {
    std::mutex m;
    for_each_cell( [&]( const Cell<Config> &cell, int ) {
        m.lock();
        cell.display_vtk( vo );
        m.unlock();
    }, weights );
}

Vec<Vec<SdotSolver::TF>> SdotSolver::matrix_ap( Span<TF> weights, TF eps ) {
    Vec<Vec<TF>> dense( FromSizeAndItemValue(), nb_cells(), FromSizeAndItemValue(), nb_cells(), 0 );
    Vec<TF> base_measures = measures( weights );
    for( PI c = 0; c < nb_cells(); ++c ) {
        TF old = std::exchange( weights[ c ], weights[ c ] + eps );

        Vec<TF> new_measures = measures( weights );
        for( PI r = 0; r < nb_cells(); ++r )
            dense[ r ][ c ] = ( new_measures[ r ] - base_measures[ r ] ) / eps;

        weights[ c ] = old;
    }
    // for
    return dense;
}

SdotSolver::TF SdotSolver::max_alpha_for( Span<TF> weights, Span<TF> dir ) {
    using std::min;

    Vec<TF> res( FromSizeAndItemValue(), Pd::max_nb_threads(), std::numeric_limits<TF>::max() );
    for_each_cell( [&]( const Cell<Config> &cell, int num_thread ) {
        TF &loc = res[ num_thread ];

        TF sum = 0;
        TF m = cell.for_each_cut_with_measure( [&]( const CellCut<Config> &cut, TF measure ) {
            if ( cut.type != CutType::Dirac )
                return;
            TF der = measure / ( 2 * norm_2( cut.p1 - cell.p0 ) );
            sum += der * ( dir[ cell.i0 ] - dir[ cut.i1 ] );
        } );

        if ( sum < 0 )
            loc = min( loc, - m / sum );
    }, weights );

    for( PI i = 1; i < res.size(); ++i )
        res[ 0 ] = min( res[ 0 ], res[ i ] );

    return res[ 0 ];
}

SdotSolver::TM SdotSolver::matrix( Span<TF> weights ) {
    // data for each thread
    struct Partial { Vec<PI> num_cells, offsets, columns; Vec<TF> values; };
    Vec<Partial> partials( FromSize(), Pd::max_nb_threads() );
    for( Partial &partial : partials ) {
        partial.num_cells.reserve( nb_cells() / partials.size() );
        partial.offsets.reserve( nb_cells() / partials.size() );
        partial.columns.reserve( 22 * nb_cells() / partials.size() );
        partial.values.reserve( 22 * nb_cells() / partials.size() );
    }

    // computations
    for_each_cell( [&]( const Cell<Config> &cell, int num_thread ) {
        Partial &partial = partials[ num_thread ];

        partial.num_cells << cell.i0;
        partial.offsets << partial.values.size();

        TF sum = 0;
        cell.for_each_cut_with_measure( [&]( const CellCut<Config> &cut, TF measure ) {
            if ( cut.type != CutType::Dirac )
                return;
            TF der = measure / ( 2 * norm_2( cut.p1 - cell.p0 ) );
            partial.columns << cut.i1;
            partial.values << - der;
            sum += der;
        } );
        
        partial.columns << cell.i0;
        partial.values << sum;
    }, weights );

    for( Partial &partial : partials )
        partial.offsets << partial.values.size();

    // merge
    TM res;
    res.offsets.resize( nb_cells() + 1 );
    for( const Partial &partial : partials )
        for( PI n = 0; n < partial.num_cells.size(); ++n )
            res.offsets[ partial.num_cells[ n ] ] = partial.offsets[ n + 1 ] - partial.offsets[ n + 0 ];
    res.offsets.back() = 0;

    for( PI v = 0, acc = 0; v < res.offsets.size(); ++v )
        acc += std::exchange( res.offsets[ v ], acc );

    res.columns.resize( res.offsets.back() );
    res.values.resize( res.offsets.back() );
    for( const Partial &partial : partials ) {
        for( PI n = 0; n < partial.num_cells.size(); ++n ) {
            PI o = res.offsets[ partial.num_cells[ n ] ];
            for( PI p = partial.offsets[ n + 0 ]; p < partial.offsets[ n + 1 ]; ++p, ++o ) {
                res.columns[ o ] = partial.columns[ p ];
                res.values[ o ] = partial.values[ p ];
            }
        }
    }

    return res;
}
