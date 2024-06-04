#pragma once

#include "../common_ctor_selectors.h"
#include "../PrimitiveCtIntList.h"
#include "../common_macros.h"
#include "../StaticSizesOf.h"
#include "../ArrayTypeFor.h"
#include "../ItemTypeOf.h"

BEG_METIL_NAMESPACE

auto make_array_from_binary_operations( auto &&functor, auto &&a, auto &&b ) {
    using Da = DECAYED_TYPE_OF( a );
    using Db = DECAYED_TYPE_OF( b );

    using Sa = StaticSizesOf<Da>::value;
    using Sb = StaticSizesOf<Db>::value;
    using Sr = DECAYED_TYPE_OF( max( Sa(), Sb() ) );

    constexpr auto na = Sa::size;
    constexpr auto nb = Sb::size;

    using Ia = ItemTypeOf<Da>::value;
    using Ib = ItemTypeOf<Db>::value;
    using Ir = DECAYED_TYPE_OF( functor( *(const Ia *)nullptr, *(const Ib *)nullptr ) );

    using ArrayType = ArrayTypeFor<Ir,Sr,1>::value;

    return ArrayType( FromOperationOnItemsOf(), FORWARD( functor ), PrimitiveCtIntList<na,nb>(), FORWARD( a ), FORWARD( b ) );
}

END_METIL_NAMESPACE

