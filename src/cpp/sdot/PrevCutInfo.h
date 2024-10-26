#pragma once

#include "Config.h"

namespace power_diagram {
PD_CLASS_DECL_AND_USE( PrevCutInfo );
PD_CLASS_DECL_AND_USE( PavingItem );

/**
 * @brief 
 * 
 */
class PD_NAME( PrevCutInfo ) {
public:
    using Data = Vec<std::pair<PavingItem *,PI32>>;
    
    PI32  find ( PavingItem *leaf ) const { for( const auto &p: by_leaf ) if ( p.first == leaf ) return p.second; return 0; }

    Data  by_leaf;
};

} // namespace power_diagram
