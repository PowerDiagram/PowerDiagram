#pragma once

#include <tl/support/memory/UniquePtr.h>
#include "WeightsWithBounds.h"
#include "DiracVec.h"
#include "Cell.h"

namespace power_diagram {
PD_CLASS_DECL_AND_USE( PavingWithDiracs );

/**
 * @brief Abstract class to describe a set of point in connected sub-regions
 * 
 */
class PD_NAME( PavingWithDiracs ) { STD_TL_TYPE_INFO( PavingWithDiracs, "" )
public:
    struct       CreationParameters {  };
    struct       CellTraversalError {  };

    static auto  New                ( const DiracVec &dv, const CreationParameters &cp = {} ) -> UniquePtr<PavingWithDiracs>;
    
    virtual int  max_nb_threads     () const;
    virtual int  for_each_cell      ( const Cell &base_cell, const WeightsWithBounds &wwb, const std::function<void( Cell &cell, int num_thread )> &f ) = 0;
    
    void         spawn              ( const std::function<void( int num_thread, int nb_threads )> &f ) const;
};

} // namespace power_diagram