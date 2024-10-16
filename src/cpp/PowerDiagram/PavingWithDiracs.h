#pragma once

#include <tl/support/memory/UniquePtr.h>
#include "DiracVec.h"
#include "Cell.h"

namespace power_diagram {
PD_CLASS_DECL_AND_USE( PavingWithDiracs );

/**
 * @brief Abstract class to describe a set of point in connected sub-regions
 * 
 */
class PD_NAME( PavingWithDiracs ) { STD_TL_TYPE_INFO( PavingWithDiracs, "", yo )
public:
    struct      CreationParameters {  };

    static auto New                ( const DiracVec &dv, const CreationParameters &cp = {} ) -> UniquePtr<PavingWithDiracs>;
    
    void        for_each_cell      ( const std::function<void( Cell )> &f );

    //void display(  ) 
    int         yo = 17;
};

} // namespace power_diagram