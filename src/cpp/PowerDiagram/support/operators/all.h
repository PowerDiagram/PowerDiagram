#pragma once

#include "../TensorOrder.h"
#include "../TypeConfig.h"

BEG_METIL_NAMESPACE

/// seq any
bool all( auto &&a ) requires ( TensorOrder<DECAYED_TYPE_OF( a )>::value == 1 ) {
    bool res = true;
    for( PI i = 0; i < a.size(); ++i )
        res &= a[ i ];
    return res;
}

END_METIL_NAMESPACE
