#include <tl/support/operators/product.h>
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

RegularGrid::PD_NAME( RegularGrid )( const DiracVec &dv, const Periodicity &periodicity ) {
    //
    auto mima = dv.global_min_max();
    min_pos = mima[ 0 ];
    max_pos = mima[ 1 ];

    //
    PI target_size = max( dv.global_size() / 16, 1ul );
    TF step = product( max_pos - min_pos ) / pow( target_size, double( 1 ) / nb_dims );
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

    P( offsets );
}

int RegularGrid::for_each_cell( const Cell &base_cell, const WeightsWithBounds &wwb, const std::function<void( Cell &cell, int num_thread )> &f ) {
    // make a base cell

    // for each box...
    int error = 0;
    spawn( [&]( int num_thread, int nb_threads ) {
        try {
            PI beg_grind = end_index() * ( num_thread + 0 ) / nb_threads;
            PI end_grind = end_index() * ( num_thread + 1 ) / nb_threads;

            for( PI grind = beg_grind; grind < end_grind; ++grind ) {
                for( PI i = offsets[ grind + 0 ]; i < offsets[ grind + 1 ]; ++i ) {
                    if ( error )
                        return;

                    P( num_thread, points[ i ] );
                }
            }
        } catch ( CellTraversalError ) {
            error = 1;
        }
    } );
    return error;
}

} // namespace power_diagram
