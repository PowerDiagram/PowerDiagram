#pragma once

#include "DiracVec.h"
#include "Mpi.h"

namespace power_diagram {
PD_CLASS_DECL_AND_USE( DiracVecFromGloballyKnownValues );

/**
 * @brief 
 * 
 */
class PD_NAME( DiracVecFromGloballyKnownValues ) : public DiracVec {
public:
    /**/        PD_NAME( DiracVecFromGloballyKnownValues )( Vec<Pt> &&values ) : values( std::move( values ) ) { init_local_bounds(); }
    /**/        PD_NAME( DiracVecFromGloballyKnownValues )( Span<Pt> values ) : values( values ) { init_local_bounds(); }

    auto        random           ( PI nb_diracs ) -> DiracVecFromGloballyKnownValues;

    virtual PI  local_size       () const { return local_end - local_beg; }
    virtual PI  size             () const { return values.size(); }
    virtual Pt  pos              ( PI index ) const = 0;

private: 
    void        init_local_bounds() { local_beg = values.size() * ( mpi->rank() + 0 ) / mpi->size(); local_end = values.size() * ( mpi->rank() + 1 ) / mpi->size(); }

    PI          local_beg;
    PI          local_end;
    Vec<Pt>     values;
};

} // namespace power_diagram
