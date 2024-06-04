#pragma once

#include "metil_namespace.h"
#include <iterator>

BEG_METIL_NAMESPACE

void for_each_item( auto &&object, auto &&f ) requires (
            requires { std::begin( object ); std::end( object ); } ||
            requires { object.for_each_item( f ); }
        ) {
    if constexpr ( requires { object.for_each_item( f ); } ) {
        object.for_each_item( f );
        return;
    } else {
        for( auto &&value : object )
            f( value );
    }
}

END_METIL_NAMESPACE
