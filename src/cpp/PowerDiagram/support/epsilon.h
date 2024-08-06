#pragma once

#include "CtType.h"
#include <limits>

BEG_METIL_NAMESPACE

TT auto epsilon( CtType<T> ) {
    return std::numeric_limits<T>::epsilon();
}

TT auto epsilon() {
    return epsilon( CtType<T>() );
}

END_METIL_NAMESPACE
