#pragma once

#include "common_macros.h"
#include "CtInt.h"

BEG_METIL_NAMESPACE

///
template<int... A>
struct CtIntList;

// specialization for several arguments
template<int H,int... T>
struct CtIntList<H,T...> {
    using                    Tail                 = CtIntList<T...>;

    static void              for_each_template_arg( auto &&f ) { f( METIL_NAMESPACE::CtInt<H>() ); ( f( METIL_NAMESPACE::CtInt<T>() ), ... ); }
    static auto              template_type_name   () { return "CtIntList"; }
    static constexpr auto    reduction            ( const auto &f, auto &&v ) { return Tail::reduction( f, f( CtInt<H>(), v ) ); }
    static constexpr bool    contains             ( int value ) { return H == value || Tail::contains( value ); }
    static auto              apply                ( auto &&f, auto... values ) { return Tail::apply( FORWARD( f ), values..., CtInt<H>() ); }
    Ti static constexpr auto with                 ( CtInt<i> ) { return CtIntList<H,T...,i>{}; }
    static constexpr auto    size                 () { return CtInt<1 + sizeof...( T )>(); }
    static constexpr auto    tail                 () { return Tail{}; }
    Ti static constexpr auto get                  ( CtInt<i> ) { if constexpr ( i ) return Tail::get( CtInt<i-1>{} ); else return CtInt<H>{}; }
    Ti static constexpr auto get                  () { return get( CtInt<i>{} ); }
};

// specialization for 0 arguments
template<>
struct CtIntList<> {
    static void              for_each_template_arg( auto &&f ) {}
    static auto              template_type_name   () { return "CtIntList"; }
    static constexpr auto    reduction            ( const auto &f, auto &&v ) { return FORWARD( v ); }
    static constexpr bool    contains             ( int value ) { return false; }
    static auto              apply                ( auto &&f, auto... values ) { return f( values... ); }
    Ti static constexpr auto with                 ( CtInt<i> ) { return CtIntList<i>{}; }
    static constexpr int     size                 () { return 0; }
};

// compile time range
Ti auto ct_range( CtInt<i> ci ) { if constexpr( i ) return ct_range( CtInt<i-1>{} ).with( CtInt<i-1>{} ); else return CtIntList<>{}; }
Ti auto ct_range() { return ct_range( CtInt<i>{} ); }

template<int...values>
auto as_ct( CtIntList<values...> v ) { return v; }

END_METIL_NAMESPACE
