#pragma once

#include "common_ctor_selectors.h"
#include "WithDefaultOperators.h"
#include "common_concepts.h"
#include "common_types.h"
#include "CtType.h"
#include "CtInt.h"
#include "Span.h"

BEG_METIL_NAMESPACE

/// basic sequence of Items with potentially static size and potential room for local data
/// Alignement is specified in bytes and can be = 0.
///
///
/// This specialization is for static vectors
template<class Item,int static_size=-1>
class Vec : public WithDefaultOperators {
public:
    // static auto      with_item_type        ( auto item_type ) { return CtType< Vec<typename VALUE_IN_DECAYED_TYPE_OF(item_type),static_size> >{}; }

    Tis                 Vec                   ( FromOperationOnItemsOf, auto &&functor, PrimitiveCtIntList<i...>, auto &&...lists );
    /**/                Vec                   ( FromItemValues, auto &&...values );
    /**/                Vec                   ( FromItemValue, auto &&...ctor_args );
    /**/                Vec                   ( FromIterator, auto iter );
    TT                  Vec                   ( const std::initializer_list<T> &lst );
    /**/                Vec                   ( const HasSizeAndAccess auto &l );
    /**/                Vec                   ( const Vec &that );
    /**/                Vec                   ( Vec && );
    /**/                Vec                   ();
    /**/               ~Vec                   ();

    Vec&                operator=             ( const Vec & );
    Vec&                operator=             ( Vec && );

    operator            Span<Item,static_size>() { return { data() }; }
    operator            Span<Item,-1>         () { return { data(), size() }; }

    auto                without_index         ( PI index ) const -> Vec<Item,static_size-1>;

    const Item&         operator[]            ( PI index ) const;
    Item&               operator[]            ( PI index );
    const Item&         operator()            ( PI index ) const;
    Item&               operator()            ( PI index );
    PI                  size_tot              () const { return size(); }
    const Item*         begin                 () const { return data(); }
    Item*               begin                 () { return data(); }
    const Item*         data                  ( PI index ) const;
    Item*               data                  ( PI index );
    const Item*         data                  () const;
    Item*               data                  ();
    const Item&         back                  () const { return operator[]( size() - 1 ); }
    Item&               back                  () { return operator[]( size() - 1 ); }
    const Item*         end                   () const { return begin() + size(); }
    Item*               end                   () { return begin() + size(); }

    constexpr auto      size                  ( PI d ) const -> CtInt<static_size> { return {}; }
    constexpr auto      size                  () const -> CtInt<static_size> { return {}; }

    static constexpr PI nbch                  = static_size * sizeof( Item );
    char                data_                 [ nbch ]; ///<
};

// dynamic size, items fully on the heap
template<class Item>
class Vec<Item,-1> : public WithDefaultOperators {
public:
    /**/                Vec             ( FromSizeAndInitFunctionOnIndex, PI size, auto &&func );
    /**/                Vec             ( FromSizeAndFunctionOnIndex, PI size, auto &&func );
    Tis                 Vec             ( FromOperationOnItemsOf, auto &&functor, PrimitiveCtIntList<i...>, auto &&...lists );
    /**/                Vec             ( FromSizeAndItemValue, PI size, auto &&...ctor_args );
    /**/                Vec             ( FromSizeAndIterator, PI size, auto iterator );
    /**/                Vec             ( FromReservationSize, PI capa, PI raw_size = 0 );
    /**/                Vec             ( FromConcatenation, auto &&...vecs );
    /**/                Vec             ( FromItemValues, auto &&...values );
    /**/                Vec             ( FromSize, PI size );
    /**/                Vec             ( const std::initializer_list<Item> &l );
    /**/                Vec             ( const HasSizeAndAccess auto &l );
    /**/                Vec             ( const Vec & );
    /**/                Vec             ( Vec && );
    /**/                Vec             ();
    /**/               ~Vec             ();

    static Vec          range           ( Item end );

    Vec&                operator=       ( const Vec &that );
    Vec&                operator=       ( Vec &&that );

    operator            Span<Item>      () { return { data(), size() }; }

    Vec&                operator<<      ( auto &&value ) { push_back( FORWARD( value) ); return *this; }

    const Item&         operator[]      ( PI index ) const;
    Item&               operator[]      ( PI index );
    const Item&         operator()      ( PI index ) const;
    Item&               operator()      ( PI index );
    PI                  size_tot        () const { return size(); }
    const Item*         begin           () const { return data(); }
    Item*               begin           () { return data(); }
    const Item*         data            ( PI index ) const;
    Item*               data            ( PI index );
    const Item*         data            () const;
    Item*               data            ();
    const Item&         back            () const { return operator[]( size() - 1 ); }
    Item&               back            () { return operator[]( size() - 1 ); }
    const Item*         end             () const { return begin() + size(); }
    Item*               end             () { return begin() + size(); }

    Item*               moved_data      (); ///<
    Span<Item>          moved_span      (); ///<

    bool                contains        ( const auto &v ) const { for( const auto &r : *this ) if ( r == v ) return true; return false; }
    bool                empty           () const { return size_ == 0; }
    PI                  size            ( PI d ) const { return size(); }
    PI                  size            () const;

    Item*               push_back_unique( auto &&value );
    void                remove_indices  ( auto &&func );
    PI                  push_back_ind   ( auto&&...args ); ///< push_back with Item{ FORWARD( args )... }, return index of new item
    Item                pop_back_val    ();
    Item*               push_back_br    ( auto&&...args ); ///< push_back with Item{ FORWARD( args )... }
    Item*               push_back       ( auto&&...args ); ///< push_back with Item( FORWARD( args )... )
    void                pop_back        ();
    void                append          ( auto &&that );

    void                reserve         ( PI capa );
    void                resize          ( PI size, auto&&...ctor_args );
    void                clear           ();

    void                copy_data_to    ( void *data ) const;
    // TUV void            set_item        ( CtType<U> array_type, CtType<V> item_type, auto &&value, const auto &index ) {
    //     if ( ! reassign( operator[]( index ), FORWARD( value ) ) ) {
    //         using Dst = VALUE_IN_DECAYED_TYPE_OF( type_promote( CtString<"reassign">(),
    //             vfs_dt_impl_type( item_type, FORWARD( value ) ),
    //             CtType<Item>()
    //         ) );
    //         P( type_name( CT_DECAYED_TYPE_OF( value ) ), type_name<Dst>() );
    //         throw typename U::template TypeException<Vec<Dst>,Vec>{ std::move( *this ) };
    //     }
    // }
    const Item&         get_item        ( const auto &index ) const { return operator[]( index ); }

    static Item*        allocate        ( PI nb_items );

private:
    Item*               data_;          ///<
    PI                  size_;          ///<
    PI                  capa_;          ///<
};

END_METIL_NAMESPACE

#define DTP template<class Item,int static_size>
#define UTP Vec<Item,static_size>

DTP auto get_compilation_flags( auto &cn, CtType<UTP> ) { cn.add_inc_file( "metil/containers/Vec.h" ); }
DTP void for_each_template_arg( CtType<UTP>, auto &&f ) { f( CtType<Item>() ); f( CtInt<static_size>() ); }
DTP auto template_type_name   ( CtType<UTP> ) { return "Vec"; }

template<class Item,int static_size> struct ArrayTypeFor<Item,PrimitiveCtIntList<static_size>,1> { using value = Vec<Item,static_size>; };
DTP struct StaticSizesOf<UTP> { using value = PrimitiveCtIntList<static_size>; };
DTP struct TensorOrder<UTP> { enum { value = 1 }; };
DTP struct ItemTypeOf<UTP> { using value = Item; };


#undef DTP
#undef UTP

#include "Vec.tcc" // IWYU pragma: export
