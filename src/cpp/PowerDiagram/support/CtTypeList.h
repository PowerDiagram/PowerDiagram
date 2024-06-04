#pragma once

#include "CtType.h"

BEG_METIL_NAMESPACE

///
template<class... A>
struct CtTypeList;

// specialization for several arguments
template<class H,class... T>
struct CtTypeList<H,T...> {
    using                    Head            = H;

    static H&                make_ref_on_head() { return *reinterpret_cast<H *>( nullptr ); }
    static constexpr auto    reduction       ( const auto &f, auto &&v ) { return tail().reduction( f, f( CtType<H>(), v ) ); }
    static auto              get_type        ( std::size_t num, auto &&func ) { if constexpr ( sizeof...( T ) ) return num ? CtTypeList<T...>::get_type( num - 1, FORWARD( func ) ) : func( CtType<H>() ); else return func( CtType<H>() ); }
    TU static constexpr auto with            ( CtType<U> ) { return CtTypeList<H,T...,U>{}; }
    static constexpr int     size            () { return 1 + sizeof...( T ); }
    static constexpr auto    tail            () { return CtTypeList<T...>{}; }
    static constexpr auto    head            () { return CtType<H>{}; }
    static constexpr auto    map             ( const auto &f ) {
        using N = VALUE_IN_DECAYED_TYPE_OF( f( CtType<H>() ) );
        return CtTypeList<N,typename VALUE_IN_DECAYED_TYPE_OF( f( CtType<T>() ) )...>();
    }
};

// specialization for 0 arguments
template<>
struct CtTypeList<> {
    static constexpr auto    reduction       ( const auto &f, auto &&v ) { return FORWARD( v ); }
    static auto              get_type        ( std::size_t num, auto &&func ) {}
    TU static constexpr auto with            ( CtType<U> ) { return CtTypeList<U>{}; }
    static constexpr int     size            () { return 0; }
};

END_METIL_NAMESPACE
