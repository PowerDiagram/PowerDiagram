#pragma once

#include <functional>
#include "ASSERT.h"
#include "Vec.h"

/// return false to stop, true to continue
inline bool for_each_selection_cont( const std::function<bool( const Vec<PI> & )> &f, PI n_sel, PI n_tot ) {
    // using TF = std::function<bool( const Vec<PI> & )>;
    Vec<PI> comb( FromSize(), n_sel );
    ASSERT( n_sel <= n_tot );

    std::function<bool(PI)> _for_each_selection = [&]( PI n_val ) -> bool {
        if ( n_val == n_sel )
            return f( comb );

        for( PI v = n_val ? comb[ n_val - 1 ] + 1 : 0; v < n_tot + n_val - n_sel + 1; ++v ) {
            comb[ n_val ] = v;
            if ( ! _for_each_selection( n_val + 1 ) )
                return false;
        }

        return true;
    };

    return _for_each_selection( 0 );
}

inline void for_each_selection( const std::function<void( const Vec<PI> & )> &f, PI n_sel, PI n_tot ) {
    for_each_selection_cont( [&]( const Vec<PI> &v ) -> bool { f( v ); return true; }, n_sel, n_tot );
}
