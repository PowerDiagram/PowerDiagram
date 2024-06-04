#pragma once

#include "TensorOrder.h"

BEG_METIL_NAMESPACE

template<class T>
concept IsScalar = TensorOrder<T>::value == 0;

END_METIL_NAMESPACE
