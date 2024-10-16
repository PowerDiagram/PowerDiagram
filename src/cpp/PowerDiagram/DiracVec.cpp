#include "DiracVec.h"
#include "Mpi.h"

namespace power_diagram {

DiracVec::~PD_NAME( DiracVec )() {
}

Vec<Pt,2> DiracVec::local_min_max() const {
    Vec<Pt,2> res;
    if ( local_size() ) {
        res[ 0 ] = local_pos( 0 );
        res[ 1 ] = res[ 0 ];

        for( PI i = 0, s = local_size(); i < s; ++i ) {
            const auto p = local_pos( i );
            res[ 0 ] = min( res[ 0 ], p );
            res[ 1 ] = max( res[ 1 ], p );
        }
    } else {
        res[ 0 ] = { FromItemValue(), 0 };
        res[ 1 ] = { FromItemValue(), 0 };
    }
    return res;
}

Vec<Pt,2> DiracVec::global_min_max() const {
    if ( mpi->size() == 1 )
        return local_min_max();
    TODO;
    // return 
}

PI DiracVec::global_size() const {
    return mpi->sum( local_size() );
}

void DiracVec::display( Displayer &ds ) const {
    ds.start_array();
    for( PI i = 0, s = local_size(); i < s; ++i )
        ds << local_pos( i );
    ds.end_array();
}

} // namespace power_diagram
