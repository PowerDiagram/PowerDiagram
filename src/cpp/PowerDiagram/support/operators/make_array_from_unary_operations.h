#pragma once

#include "../common_ctor_selectors.h"
#include "../PrimitiveCtIntList.h"
#include "../common_macros.h"
#include "../StaticSizesOf.h"
#include "../ArrayTypeFor.h"
#include "../ItemTypeOf.h"

BEG_METIL_NAMESPACE

auto make_array_from_unary_operations( auto &&functor, auto &&a ) {
    using Da = DECAYED_TYPE_OF( a );

    using Sa = StaticSizesOf<Da>::value;

    constexpr auto na = Sa::size;

    using Ia = ItemTypeOf<Da>::value;
    using Ir = DECAYED_TYPE_OF( functor( *(const Ia *)nullptr ) );

    using ArrayType = ArrayTypeFor<Ir,Sa,1>::value;

    return ArrayType( FromOperationOnItemsOf(), FORWARD( functor ), PrimitiveCtIntList<na>(), FORWARD( a ) );
}

END_METIL_NAMESPACE
