#pragma once

#include "string/read_arg_name.h"

BEG_METIL_NAMESPACE

/// as a method
void for_each_attribute( const auto &object, auto &&f ) requires requires { object.for_each_attribute( f ); } {
    object.for_each_attribute( f );
}

void parse_attributes( auto &&f, StrView names, auto&&...attrs ) {
    ( f( read_arg_name( names ), FORWARD( attrs ) ), ... );
}

END_METIL_NAMESPACE
