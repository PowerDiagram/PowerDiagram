#pragma once

#include "common_macros.h"
#include <type_traits>

BEG_METIL_NAMESPACE

/// A structure to store a compile time known type
template<class Value>
struct CtType {
    using         value                = Value;

    static void   for_each_template_arg( auto &&f ) { f( CtType<Value>() ); }
    static auto   template_type_name   () { return "CtType"; }
    static auto   to_string            (); ///< defined in type_name.h
    static auto   fake_ptr             () -> std::decay_t<Value> * { return nullptr; }
    static auto*  display              ( auto &ds ); ///< defined in type_name.h
    const auto*   cast                 ( const auto *v ) const { return reinterpret_cast<const value *>( v ); }
    auto*         cast                 ( auto *v ) const { return reinterpret_cast<value *>( v ); }
};

/// concept to test if a type is a CtType of something
template<class U>
concept IsA_CtType = std::is_same_v<std::decay_t<U>,CtType<typename std::decay_t<U>::value>>;

template<class Value>
auto as_ct( CtType<Value> v ) { return v; }

/// make an instance of CtType<type of v> if v is known.
// auto actual_type_of( const auto &v ) { return CT_DECAYED_TYPE_OF( v ); }

END_METIL_NAMESPACE
