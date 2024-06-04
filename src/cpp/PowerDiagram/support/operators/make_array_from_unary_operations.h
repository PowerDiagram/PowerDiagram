#pragma once

// #include "../support/STATIC_ASSERT_IN_IF_CONSTEXPR.h"
// #include "../support/call_by_name.h"
#include "../common_types.h"

BEG_METIL_NAMESPACE

// auto make_ArrayImpl_from_unary_operations( auto op_name, auto &&a ) {
//     using DA = DECAYED_TYPE_OF( a );
//     if constexpr ( requires { a.~Vec(); } ) {
//         auto item_type = CT_DECAYED_TYPE_OF( call_by_name( op_name, a[ 0 ] ) );
//         using Res = VALUE_IN_DECAYED_TYPE_OF( VecType_for( item_type, a.size() ) );
//         return Res( FromOperationOnItemsOf(), op_name, CtIntList<1>(), FORWARD( a ) );
//     } else
//         STATIC_ASSERT_IN_IF_CONSTEXPR( 0, "make_ArrayImpl_from_unary_operations" );
// }

END_METIL_NAMESPACE
