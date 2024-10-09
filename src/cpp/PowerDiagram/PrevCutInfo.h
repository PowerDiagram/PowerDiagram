#pragma once

#include <tl/support/containers/Vec.h>

template<class Config> class PointTree;

/**
 * @brief 
 * 
 */
template<class Config>
class PrevCutInfo {
public:
    using Data = Vec<std::pair<PointTree<Config> *,PI32>>;
    
    PI32  find ( PointTree<Config> *leaf ) const {
        for( const auto &p: by_leaf )
            if ( p.first == leaf )
                return p.second;
        return 0;
    }

    Data  by_leaf;
};
