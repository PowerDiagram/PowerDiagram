#include "DiracVecFromLocallyKnownValues.h"
#include "Mpi.h"

namespace sdot {

DiracVecFromLocallyKnownValues::PD_NAME( DiracVecFromLocallyKnownValues )( Vec<Pt> &&values ) : values( std::move( values ) ) {
}

DiracVecFromLocallyKnownValues::PD_NAME( DiracVecFromLocallyKnownValues )( Span<Pt> values ) : values( values ) {
}

DiracVecFromLocallyKnownValues::~PD_NAME( DiracVecFromLocallyKnownValues )() {
}

DiracVecFromLocallyKnownValues DiracVecFromLocallyKnownValues::random( PI nb_diracs_tot, Pt min_pos, Pt max_pos ) {
    PI beg = nb_diracs_tot * ( mpi->rank() + 0 ) / mpi->size();
    PI end = nb_diracs_tot * ( mpi->rank() + 1 ) / mpi->size();
    return Vec<Pt>{ FromSizeAndInitFunctionOnIndex(), end - beg, [&]( Pt *res, PI ) {
        new ( res ) Pt( FromInitFunctionOnIndex(), [&]( TF *val, PI d ) {
            new ( val ) TF( min_pos[ d ] + rand() * ( max_pos[ d ] - min_pos[ d ] ) / RAND_MAX );
        } );
    } };
}

DiracVecFromLocallyKnownValues DiracVecFromLocallyKnownValues::random( PI nb_diracs_tot ) {
    return random( nb_diracs_tot, Pt( FromItemValue(), 0 ), Pt( FromItemValue(), 1 ) );
}

PI DiracVecFromLocallyKnownValues::local_size() const {
    return values.size();
}

Pt DiracVecFromLocallyKnownValues::local_pos( PI index ) const {
    return values[ index ];
}

} // namespace sdot

