#pragma once

#include "operators/self_add.h"
#include "operators/self_sub.h"
#include "operators/self_mul.h"
#include "operators/self_div.h"
#include "operators/self_mod.h"

#include "operators/add.h"
#include "operators/sub.h"
#include "operators/mul.h"
#include "operators/div.h"
#include "operators/mod.h"

#include "operators/inf.h"
#include "operators/sup.h"
#include "operators/leq.h"
#include "operators/geq.h"
#include "operators/equ.h"
#include "operators/neq.h"

#include "operators/neg.h"

#include "operators/max.h" // IWYU pragma: export
#include "operators/min.h" // IWYU pragma: export

BEG_METIL_NAMESPACE

/// base class that permits to add operators like `operator+` as friend function that call functions like `add`, `mul`, ...
struct WithDefaultOperators {
    #define ADD_BIN_OP( OP, FUN ) friend constexpr auto OP( auto &&a,  auto &&b ) requires  \
        std::is_base_of_v<WithDefaultOperators,DECAYED_TYPE_OF(a)> || \
        std::is_base_of_v<WithDefaultOperators,DECAYED_TYPE_OF(b)> { return FUN( FORWARD( a ), FORWARD( b ) ); }
    #define ADD_UNA_OP( OP, FUN ) friend constexpr auto OP( auto &&a ) requires  \
        std::is_base_of_v<WithDefaultOperators,DECAYED_TYPE_OF(a)> { return FUN( FORWARD( a ) ); }
    #define ADD_BIN_SO( OP, FUN ) friend constexpr auto &OP( auto &a,  auto &&b ) requires  \
        std::is_base_of_v<WithDefaultOperators,DECAYED_TYPE_OF(a)> || \
        std::is_base_of_v<WithDefaultOperators,DECAYED_TYPE_OF(b)> { FUN( a, FORWARD( b ) ); return a; }
    #define ADD_UNA_SO( OP, FUN ) friend constexpr auto &OP( auto &a ) requires \
        std::is_base_of_v<WithDefaultOperators,DECAYED_TYPE_OF(a)> { FUN( FORWARD( a ) ); return a; }

    ADD_BIN_OP( operator+ , add )
    ADD_BIN_OP( operator- , sub )
    ADD_BIN_OP( operator* , mul )
    ADD_BIN_OP( operator/ , div )
    ADD_BIN_OP( operator% , mod )
    ADD_BIN_OP( operator< , inf )
    ADD_BIN_OP( operator> , sup )
    ADD_BIN_OP( operator<=, leq )
    ADD_BIN_OP( operator>=, geq )
    ADD_BIN_OP( operator==, equ )
    ADD_BIN_OP( operator!=, neq )

    ADD_UNA_OP( operator- , neg )

    ADD_BIN_SO( operator+=, self_add )
    ADD_BIN_SO( operator-=, self_sub )
    ADD_BIN_SO( operator*=, self_mul )
    ADD_BIN_SO( operator/=, self_div )
    ADD_BIN_SO( operator%=, self_mod )

    #undef ADD_BIN_OP
    #undef ADD_UNA_OP
    #undef ADD_BIN_SO
    #undef ADD_UNA_SO
};

END_METIL_NAMESPACE
