#include <tl/support/operators/product.h>
#include <tl/support/operators/norm_2.h>
#include "RegularGrid.h"
#include "Mpi.h"

namespace power_diagram {
using std::exchange;
using std::max;
using std::min;
using std::pow;

PI RegularGrid::end_index() const {
    return product( nb_divs );
}

PI RegularGrid::index( const Pt &pos, int dim ) const {
    return min( nb_divs[ dim ] - 1, PI( ( pos[ dim ] - min_pos[ dim ] ) / steps[ dim ] ) );
}

PI RegularGrid::index( const Pt &pos ) const {
    PI res = index( pos, 0 );
    for( PI d = 1, m = 1; d < nb_dims; ++d )
        res += ( m *= nb_divs[ d - 1 ] ) * index( pos, d );
    return res;
}

RegularGrid::PD_NAME( RegularGrid )( const DiracVec &dv, const Periodicity &periodicity, TF nb_diracs_per_box ) {
    //
    auto mima = dv.global_min_max();
    min_pos = mima[ 0 ];
    max_pos = mima[ 1 ];

    //
    TF step = pow( dv.global_size() / ( nb_diracs_per_box * product( max_pos - min_pos ) ), double( 1 ) / nb_dims );
    for( PI d = 0; d < nb_dims; ++d ) {
        nb_divs[ d ] = ceil( ( max_pos[ d ] - min_pos[ d ] ) / step );
        steps[ d ] = TF( max_pos[ d ] - min_pos[ d ] ) / nb_divs[ d ];
    }

    // get local nb diracs for each cell (stored in this->offsets)
    offsets.resize( product( nb_divs ) + 1, 0 );
    for( PI i = 0, s = dv.local_size(); i < s; ++i )
        ++offsets[ index( dv.local_pos( i ) ) ];

    // exclusive scan
    if ( mpi->size() > 1 )
        TODO;
    for( PI i = 0, a = 0; i < offsets.size(); ++i )
        offsets[ i ] = exchange( a, a + offsets[ i ] );

    // get points and inds
    points.resize( dv.local_size() );
    inds.resize( dv.local_size() );
    for( PI i = 0, s = dv.local_size(); i < s; ++i ) {
        Pt pt = dv.local_pos( i );

        PI o = offsets[ index( pt ) ]++;
        points[ o ] = pt;
        inds[ o ] = i;
    }

    // remake the exclusive scan
    for( PI i = 0, o = 0; i < offsets.size() - 1; ++i )
        offsets[ i ] = std::exchange( o, offsets[ i ] );
}

void RegularGrid::make_cuts_from( PI b0, PI n0, Cell &cell, Vec<PI> &buf, const WeightsWithBounds &weights ) {
    // indices of points to be sorted
    buf.clear();
    for( PI n1 = offsets[ b0 + 0 ]; n1 < offsets[ b0 + 1 ]; ++n1 )
        if ( n1 != n0 )
            buf << n1;

    std::sort( buf.begin(), buf.end(), [&]( PI a, PI b ) -> bool {
        return norm_2_p2( points[ a ] - cell.p0 ) < norm_2_p2( points[ b ] - cell.p0 );
    } );

    // make the cuts
    for( PI n1 : buf ) {
        const PI i1 = inds[ n1 ];
        cell.cut_dirac( points[ n1 ], weights[ i1 ], i1, nullptr, n1 );
    }
}

int RegularGrid::for_each_cell( const Cell &base_cell, const WeightsWithBounds &weights, const std::function<void( Cell &cell, int num_thread )> &f ) {
    // for each box...
    int error = 0;
    spawn( [&]( int num_thread, int nb_threads ) {
        try {
            PI beg_b0 = end_index() * ( num_thread + 0 ) / nb_threads;
            PI end_b0 = end_index() * ( num_thread + 1 ) / nb_threads;

            Cell local_cell;
            Vec<PI> buf( FromReservationSize(), 128 );
            for( PI b0 = beg_b0; b0 < end_b0; ++b0 ) {
                for( PI n0 = offsets[ b0 + 0 ]; n0 < offsets[ b0 + 1 ]; ++n0 ) {
                    if ( error )
                        return;
                    const PI i0 = inds[ n0 ];

                    local_cell.init_from( base_cell, points[ n0 ], weights[ i0 ], i0 );
                    make_cuts_from( b0, n0, local_cell, buf, weights );
                    f( local_cell, num_thread );
                }
            }
        } catch ( CellTraversalError ) {
            error = 1;
        }
    } );
    return error;
}

} // namespace power_diagram
