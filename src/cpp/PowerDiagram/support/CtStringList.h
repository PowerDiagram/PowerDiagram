#pragma once

#include "CtString.h" // IWYU pragma: export

BEG_VFS_NAMESPACE

///
template<CtStringValue... values>
struct CtStringList {
    static void  for_each_template_arg( auto &&f ) { ( f( CtString<values>() ), ... ); }
    static auto  template_type_name   () { return "CtStringList"; }
    static void  for_each_string      ( auto &&f ) { ( f( values ), ... ); }
    static auto *display              ( auto &ds ) { return ds.array( { ds.string( values.value )... } ); }
};

template<CtStringValue... values>
auto as_ct( CtStringList<values...> v ) { return v; }

END_VFS_NAMESPACE
