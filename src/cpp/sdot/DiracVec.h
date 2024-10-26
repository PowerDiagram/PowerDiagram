#pragma once

#include <sdot/Config.h>

namespace sdot {
PD_CLASS_DECL_AND_USE( DiracVec );

/**
 * @brief A basic abstraction of a list of Diracs... with data that may be on harddrive, other MPI processes, ...
 * 
 */
class PD_NAME( DiracVec ) {
public:
    virtual          ~PD_NAME( DiracVec )();

    virtual Vec<Pt,2> local_min_max      () const;
    virtual PI        local_size         () const = 0;
    virtual Pt        local_pos          ( PI index ) const = 0;
     
    virtual Vec<Pt,2> global_min_max     () const;
    virtual PI        global_size        () const;
      
    virtual void      display            ( Displayer &ds ) const;
};

} // namespace sdot
