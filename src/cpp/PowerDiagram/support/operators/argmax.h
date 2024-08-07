#pragma once

#include "../common_types.h"
#include "../TensorOrder.h"

BEG_METIL_NAMESPACE

/// seq argmin
PI argmax( auto &&a ) requires ( TensorOrder<DECAYED_TYPE_OF( a )>::value == 1 ) {
    PI res = 0;
    for( PI i = 1; i < a.size(); ++i )
        if ( a[ res ] < a[ i ] )
            res = i;
    return res;
}

END_METIL_NAMESPACE
