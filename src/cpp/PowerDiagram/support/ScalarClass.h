#pragma once

#include "common_types.h"

BEG_METIL_NAMESPACE

enum {
    ScalarClass_integer  = 0,
    ScalarClass_rational = 1,
    ScalarClass_symbolic = 2,
    ScalarClass_floating = 3,

};

///
TT struct ScalarClass;

template<> struct ScalarClass<PI64> { enum { value = ScalarClass_integer  }; };
template<> struct ScalarClass<PI32> { enum { value = ScalarClass_integer  }; };
template<> struct ScalarClass<PI16> { enum { value = ScalarClass_integer  }; };
template<> struct ScalarClass<PI8 > { enum { value = ScalarClass_integer  }; };

template<> struct ScalarClass<SI64> { enum { value = ScalarClass_integer  }; };
template<> struct ScalarClass<SI32> { enum { value = ScalarClass_integer  }; };
template<> struct ScalarClass<SI16> { enum { value = ScalarClass_integer  }; };
template<> struct ScalarClass<SI8 > { enum { value = ScalarClass_integer  }; };

template<> struct ScalarClass<bool> { enum { value = ScalarClass_integer  }; };

template<> struct ScalarClass<FP32> { enum { value = ScalarClass_floating }; };
template<> struct ScalarClass<FP64> { enum { value = ScalarClass_floating }; };

END_METIL_NAMESPACE
