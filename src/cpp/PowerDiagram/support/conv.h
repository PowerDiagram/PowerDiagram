#pragma once

#include "CtType.h"

template<class T>
T conv( CtType<T>, auto &&value ) {
    return FORWARD( value );
}
