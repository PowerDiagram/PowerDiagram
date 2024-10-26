#pragma once

#include "DiracVec.h"

namespace sdot {
PD_CLASS_DECL_AND_USE( DiracVecFromLocallyKnownValues );

/**
 * @brief 
 * 
 */
class PD_NAME( DiracVecFromLocallyKnownValues ) : public DiracVec {
public:
    /**/              PD_NAME( DiracVecFromLocallyKnownValues )( Vec<Pt> &&values );
    /**/              PD_NAME( DiracVecFromLocallyKnownValues )( Span<Pt> values );
    /**/             ~PD_NAME( DiracVecFromLocallyKnownValues )();
      
    static auto       random                                   ( PI nb_diracs_tot, Pt min_pos, Pt max_pos ) -> DiracVecFromLocallyKnownValues;
    static auto       random                                   ( PI nb_diracs_tot ) -> DiracVecFromLocallyKnownValues;
                         
    virtual PI        local_size                               () const override;
    virtual Pt        local_pos                                ( PI index ) const override;

private:      
    Vec<Pt>           values;
};

} // namespace sdot
