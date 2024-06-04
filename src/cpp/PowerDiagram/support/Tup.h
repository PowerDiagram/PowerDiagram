#pragma once

#include "common_ctor_selectors.h"
#include "StorageTypeFor.h"
#include "compare.h"

BEG_METIL_NAMESPACE

/// a simple replacement for tuple, providing head and next attributes
///  + values in the natural ordering
template<class... Items>
struct Tup;

//
template<class Head,class... Tail>
struct Tup<Head,Tail...> {
    TTY using                    Filtered              = std::conditional_t<Y<Head>::value,typename Tup<Tail...>::template Filtered<Y>::template Prepend<Head>,typename Tup<Tail...>::template Filtered<Y>>;
    TT using                     Prepend               = Tup<T,Head,Tail...>;
    using                        Next                  = Tup<Tail...>;
    static constexpr std::size_t size                  = 1 * Next::size;
    TTY struct                   Map                   { using value = Tup<typename Y<Head>::value,typename Y<Tail>::value...>; };

    constexpr                    Tup                   ( FromTupleValues, auto &&tuple ) : head( tuple.head ), tail( FromTupleValues(), tuple.tail ) {}
    constexpr                    Tup                   ( auto &&head, auto &&...tail ) requires ( std::is_same_v<Tup,DECAYED_TYPE_OF(head)> == false ) : head( FORWARD( head ) ), tail( FORWARD( tail )... ) {}
    constexpr                    Tup                   ( const Tup &that ) = default;
    constexpr                    Tup                   ( Tup &&that ) = default;
    constexpr                    Tup                   () = default;

    Tup&                         operator=             ( const Tup<Head,Tail...> &that ) { head = that.head; tail = that.tail; return *this;  }
    Tup&                         operator=             ( Tup<Head,Tail...> &&that ) { head = std::move( that.head ); tail = std::move( that.tail ); return *this;  }

    std::ptrdiff_t               compare               ( const Tup &t ) const { if ( auto v = METIL_NAMESPACE::compare( head, t.head ) ) return v; return tail.compare( t.tail ); }

    constexpr auto               prefix_scan_with_index( auto &&func, auto &&value_so_far, auto &&index, const auto &increment, auto &&...values ) const { auto n = index + increment; auto v = func( value_so_far, head, n ); return tail.prefix_scan_with_index( FORWARD( func ), v, std::move( n ), increment, FORWARD( values )..., std::move( value_so_far ) ); }
    constexpr auto               reduction_with_index  ( auto &&func, auto &&value_so_far, auto index, auto increment ) const { auto n = index + increment; auto v = func( value_so_far, head, n ); return tail.reduction_with_index( FORWARD( func ), std::move( v ), std::move( n ), increment ); }

    TTY void                     filtered_apply_seq    ( const auto &func ) const;
    TTY void                     filtered_apply_seq    ( const auto &func );
    TTY auto                     filtered_apply        ( auto &&func, auto &&...end_args ) const;
    TTY auto                     filtered_apply        ( auto &&func, auto &&...end_args );
    auto                         apply                 ( auto &&func, auto &&...end_args ) const { return tail.apply( FORWARD( func ), FORWARD( end_args )..., head ); }
    auto                         apply                 ( auto &&func, auto &&...end_args ) { return tail.apply( FORWARD( func ), FORWARD( end_args )..., head ); }

    auto                         reversed_tie          ( auto &...values_so_far ) const { return tail.reversed_tie( head, values_so_far... ); }

    // auto                      append                ( auto &&...args ) { return Tuple<Head,Tail...,>; }

    NUA Head                     head;
    NUA Next                     tail;
};

//
template<>
struct Tup<> {
    TTY using                    Filtered              = Tup;
    TT using                     Prepend               = Tup<T>;
    static constexpr std::size_t size                  = 0;
    TTY struct                   Map                   { using value = Tup<>; };

    constexpr                    Tup                 ( FromTupleValues, auto &&tuple ) {}
    constexpr                    Tup                 ( const Tup & ) {}
    constexpr                    Tup                 ( Tup && ) {}
    constexpr                    Tup                 () {}

    Tup&                         operator=             ( const Tup<> & ) { return *this; }
    Tup&                         operator=             ( Tup<> && ) { return *this; }

    std::ptrdiff_t               compare               ( const Tup &t ) const { return 0; }

    constexpr auto               prefix_scan_with_index( auto &&func, auto &&value_so_far, auto &&index, const auto &increment, auto &&...values ) const { return Tup<typename StorageTypeFor<decltype(values)>::value...>{ FORWARD( values )... }; }
    constexpr auto               reduction_with_index  ( auto &&func, auto &&value_so_far, auto index, auto increment ) const { return FORWARD( value_so_far ); }

    TTY void                     filtered_apply_seq    ( const auto &func ) const {}
    TTY auto                     filtered_apply        ( auto &&func, auto &&...end_args ) const { return func( FORWARD( end_args )... ); }
    auto                         apply                 ( auto &&func, auto &&...end_args ) const { return func( FORWARD( end_args )... ); }

    TA auto                      reversed_tie          ( A &...values_so_far ) const { return Tup<A& ...>{ values_so_far... }; }
};

// ctor functions ------------------------------------------------------------------------------------------------------------

///
template<class ...Args>
auto tie( Args &...args ) {
    return Tup<Args& ...>{ args... };
}

///
template<class ...Args>
auto tup( Args &&...args ) {
    return Tup<typename StorageTypeFor<Args>::value...>{ FORWARD( args )... };
}

// tuple_cat ------------------------------------------------------------------------------------------------------------------

///
template<class... A,class... B>
auto tuple_cat( Tup<A...> &&a, Tup<B...> &&b ) {
    return a.apply( [&]( auto &...va ) {
        return b.apply( [&]( auto &...vb ) {
            return Tup<A...,B...>( std::move( va )..., std::move( vb )... );
        } );
    } );
}

///
template<class... A,class... B>
auto tuple_cat( const Tup<A...> &a, const Tup<B...> &b ) {
    return a.apply( [&]( auto &...va ) {
        return b.apply( [&]( auto &...vb ) {
            return Tup<A...,B...>( va..., vb... );
        } );
    } );
}

///
auto tuple_cat( auto &&a, auto &&b, auto &&c ) { return tuple_cat( tuple_cat( a, b ), c ); }
auto tuple_cat( auto &&a ) { return a; }

// ext functions ---------------------------------------------------------------------------------------------------------------
TA auto *display( auto &ds, const Tup<A...> &value ) { return value.apply( [&]( const auto &...args ) { return ds.array( { display( ds, args )... } ); } ); }

// -----------------------------------------------------------------------------------------------------------------------------
TA struct StorageTypeFor<Tup<A...>> { using value = Tup<typename StorageTypeFor<A>::value...>; };

TA auto ensure_tup( const Tup<A...> &tup ) { return tup; }
TA auto ensure_tup( Tup<A...> &&tup ) { return std::move( tup ); }

END_METIL_NAMESPACE
