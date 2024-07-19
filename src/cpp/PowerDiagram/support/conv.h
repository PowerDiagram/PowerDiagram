#pragma once

#include "CtType.h"

template<class T,class U>
inline T conv( CtType<T>, const U &value ) {
    return FORWARD( value );
}

