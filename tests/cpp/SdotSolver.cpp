#include <tl/support/operators/norm_2.h>
#include <tl/support/operators/sum.h>
#include "SdotSolver.h"
// #include <utility>

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
        // using namespace boost::math::differentiation;
        // using AD = autodiff_fvar<TF,1>;

        // auto m = cell.measure( [&]( TF w, CutType type, PI i ) -> AD {
        //     AD res = w;
        //     if ( type == CutType::Dirac && i == cell.i0 )
        //         res += make_fvar<TF, 1>( 0 );
        //     return res;
        // } );

        // const TF target_measure = TF( 1 ) / nb_cells();
        // res[ cell.i0 ] = ( target_measure - m.derivative( 0 ) ) / m.derivative( 1 );
        TODO;
    }, weights );
    return res;
}

Vec<SdotSolver::TF,3> SdotSolver::der_err( Span<TF> weights, Span<TF> dir ) {
    // using namespace std;

    // Vec<TF> d0( FromSizeAndItemValue(), Pd::max_nb_threads(), 0 );
    // Vec<TF> d1( FromSizeAndItemValue(), Pd::max_nb_threads(), 0 );
    // Vec<TF> d2( FromSizeAndItemValue(), Pd::max_nb_threads(), 0 );
    // for_each_cell( [&]( const Cell<Config> &cell, int num_thread ) {
    //     using AD = autodiff_fvar<TF,1>;
    //     auto m = cell.measure( [&]( TF w, CutType type, PI i ) -> AD {
    //         AD res = w;
    //         if ( type == CutType::Dirac )
    //             res += make_fvar<TF,1>( 0 ) * dir[ i ];
    //         return res;
    //     } );

    //     const TF target_measure = TF( 1 ) / nb_cells();
    //     AD di = target_measure - m;

    //     d0[ num_thread ] += 1 * di.derivative( 0 ) * di.derivative( 0 );
    //     d1[ num_thread ] += 4 * di.derivative( 0 ) * di.derivative( 1 );
    //     d2[ num_thread ] += 1 * di.derivative( 1 ) * di.derivative( 1 );
    // }, weights );

    // return { sum( d0 ), sum( d1 ), sum( d2 ) };
    TODO;
}

Vec<SdotSolver::TF> SdotSolver::measures( Span<TF> weights ) {
    Vec<TF> res( FromSizeAndItemValue(), nb_cells() );
    for_each_cell( [&]( const Cell<Config> &cell, int num_thread ) {
        res[ cell.i0 ] = cell.measure();
    }, weights );
    return res;
}

SdotSolver::TF SdotSolver::error( Span<TF> weights ) {
    using std::pow;

    Vec<TF> errors( FromSizeAndItemValue(), Pd::max_nb_threads(), 0 );
    for_each_cell( [&]( const Cell<Config> &cell, int num_thread ) {
        const TF target_measure = TF( 1 ) / nb_cells();
        errors[ num_thread ] += pow( cell.measure() - target_measure, 2 );
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
        partial.num_cells << partial.values.size();
        partial.offsets << partial.values.size();

        cell.for_each_cut_with_measure( [&]( const CellCut<Config> &cut, TF measure ) {
            if ( cut.type != CutType::Dirac )
                return;
            TF der = measure / norm_2( cut.p1 - cell.p0 );
            partial.columns << cut.i1;
            partial.values << der;
        } );
    }, weights );

    for( Partial &partial : partials )
        partial.offsets << partial.values.size();

    // merge
    TM res;
    res.offsets.resize( nb_cells() );
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
