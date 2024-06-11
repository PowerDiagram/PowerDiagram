#pragma once

#include "../string/read_arg_name.h"
#include "../for_each_attribute.h"
#include "../BumpPointerPool.h"
#include "../for_each_item.h"
#include "../TODO.h"
#include "../Vec.h"

#include "DisplayObjectFiller.h"
#include "DisplayArrayFiller.h"
#include "DisplayTypeInfo.h"
#include "DisplayPtrId.h"

#include <variant>
#include <map>

BEG_METIL_NAMESPACE
class DisplayItem;

/**
 * helper class to create DisplayItems in a pool
 *
 */
class DisplayItemFactory {
public:
    using           PointerMap        = std::map<DisplayPtrId,DisplayItem *>;
    struct          NumberInfo        { Str numerator, denominator, exponant, precision; /* base 10 representations */ };

    // type info
    static Str      include_path      ();
    static Str      type_name         ();

    // generic version
    DisplayItem*    new_display_item  ( const auto &value ); ///<

    // creation of items
    DisplayItem*    new_object        ( DisplayTypeInfo ti, const std::function<void( DisplayObjectFiller &dof )> &add_attributes ); ///< full version where everything is specified explicitely
    DisplayItem*    new_object        ( const auto &object ) { return new_object( CT_DECAYED_TYPE_OF( object ), [&]( DisplayObjectFiller &dof ) { for_each_attribute( object, [&]( const auto &name, const auto &value ) { dof.add( name, value ); } ); } ); } ///< use of type_name<Object>() and call for_each_attribute( object, ... )

    DisplayItem*    new_number        ( DisplayTypeInfo ti, NumberInfo number ); ///<

    DisplayItem*    new_string        ( DisplayTypeInfo ti, StrView str ); ///<

    DisplayItem*    new_symbol        ( DisplayTypeInfo ti, StrView str ); ///<

    DisplayItem*    new_pointer       ( DisplayTypeInfo ti, DisplayPtrId pid, Vec<Str> port_path, Str label, Str style, const std::function<DisplayItem *()> &get_content ); ///< full version where everything is specified explicitely
    DisplayItem*    new_pointer       ( const auto &object ) { return new_pointer( CT_DECAYED_TYPE_OF( object ), object, {}, "", "", [&]() { return new_display_item( *object ); } ); }

    DisplayItem*    new_array         ( DisplayTypeInfo ti, const std::function<void( DisplayArrayFiller &dof )> &add_attributes ); ///< full version where everything is specified explicitely
    DisplayItem*    new_array         ( const auto &object ) { return new_array( CT_DECAYED_TYPE_OF( object ), [&]( DisplayArrayFiller &dof ) { for_each_item( object, [&]( const auto &value ) { dof.add( value ); } ); } ); } ///< use of type_name<Object>() and call for_each_attribute( object, ... )

    // output
    void            write_pointers    ( const std::function<void( std::string_view str )> &func, DisplayWriteContext &ctx ) const;
    void            show              ( DisplayItem *root_item ) const;

    // context
    void            start_src_pos_info( StrView filename, int line, int col ); ///<
    void            end_src_pos_info  (); ///<

    //
    PointerMap      pointer_map;
    BumpPointerPool pool;
};

// =======================================================================================================================================
DisplayItem *display( DisplayItemFactory &ds, const Str&         str );
DisplayItem *display( DisplayItemFactory &ds, StrView            str );
DisplayItem *display( DisplayItemFactory &ds, const char*        str );
DisplayItem *display( DisplayItemFactory &ds, char               str );

DisplayItem *display( DisplayItemFactory &ds, PI64               val );
DisplayItem *display( DisplayItemFactory &ds, PI32               val );
DisplayItem *display( DisplayItemFactory &ds, PI16               val );
DisplayItem *display( DisplayItemFactory &ds, PI8                val );

DisplayItem *display( DisplayItemFactory &ds, SI64               val );
DisplayItem *display( DisplayItemFactory &ds, SI32               val );
DisplayItem *display( DisplayItemFactory &ds, SI16               val );
DisplayItem *display( DisplayItemFactory &ds, SI8                val );

DisplayItem *display( DisplayItemFactory &ds, bool               val );

DisplayItem *display( DisplayItemFactory &ds, FP80               val );
DisplayItem *display( DisplayItemFactory &ds, FP64               val );
DisplayItem *display( DisplayItemFactory &ds, FP32               val );

DisplayItem *display( DisplayItemFactory &ds, const void*        val );
DisplayItem *display( DisplayItemFactory &ds, void*              val );

// std::pair
template<class A,class B>
DisplayItem *display( DisplayItemFactory &ds, const std::pair<A,B> &value ) {
    return ds.new_array( CtType<std::pair<A,B>>(), [&]( DisplayArrayFiller &daf ) { daf.add( value.first ); daf.add( value.second ); } );
}

// std::tuple
template<class... A>
DisplayItem *display( DisplayItemFactory &ds, const std::tuple<A...> &value ) {
    return ds.new_array( CtType<std::tuple<A...>>(), [&]( DisplayArrayFiller &daf ) {
        return std::apply( [&]( const auto &...args ) {
            ( daf.add( args ), ... );
        }, value );
    } );
}

// std::variant
template<class... A>
DisplayItem *display( DisplayItemFactory &ds, const std::variant<A...> &value ) {
    return std::visit( [&]( const auto &v ) {
        return ds.new_display_item( v );
    }, value );
}

// generic
template<class T>
DisplayItem *display( DisplayItemFactory &ds, const T &value ) {
    // value.display
    if constexpr( requires { value.display( ds ); } ) {
        return value.display( ds );
    }
    // for_each_attribute (for objects)
    else if constexpr( requires { for_each_attribute( value, []( auto&& ) {} ); } ) {
        return ds.new_object( value );
    }
    // for_each_item (for arrays)
    else if constexpr( requires { for_each_item( value, []( const auto & ) {} ); } ) {
        return ds.new_array( value );
    }
    // *value
    else if constexpr( requires { bool( value ), *value; } ) {
        return ds.new_pointer( value );
    }
    // value.to_string
    else if constexpr( requires { value.to_string(); } ) {
        return ds.new_display_item( value.to_string() );
    }
    // os << ...
    else if constexpr( requires ( std::ostream &os ) { os << value; } ) {
        // std::ostringstream ss;
        // ss << value;
        // return ds.display( ss.str() );
        TODO;
    }
    // T::template_type_name() (for empty structures)
    else if constexpr( requires { T::template_type_name(); } ) {
        // return ds.display( T::template_type_name() );
        TODO;
    }
    // T::type_name() (for empty structures)
    else if constexpr( requires { T::type_name(); } ) {
        // return ds.display( T::type_name() );
        TODO;
    }
    // value.display again (to get an error message)
    else
        return value.display( ds );
}

// // DS_OBJECT
// template<class...A>
// void _ds_object( DisplayItemFactory &ds, StrView type, StrView attr_names, const A &...args ) {
//     auto add_item = [&]( auto &&arg_value ) { ds.set_next_name( read_arg_name( attr_names ) ); ds.display( arg_value ); };

//     ds.set_next_type( type );
//     ds.start_array_item();
//     ( add_item( args ), ... );
//     ds.end_item();
// }

#define DS_OBJECT( NAME, ... ) \
    ds.new_object( CtType<NAME>(), [&]( DisplayObjectFiller &dof ) { dof.add( #__VA_ARGS__, __VA_ARGS__ ); } )

inline DisplayItem *DisplayItemFactory::new_display_item( const auto &value ) {
    return METIL_NAMESPACE::display( *this, value );
}

// =======================================================================================================================================
void DisplayObjectFiller::add( StrView name, auto&&...items ) {
    ( add( read_arg_name( name ), display( *ds, items ) ), ... );
}

void DisplayArrayFiller::add( auto &&item ) {
    add( display( *ds, item ) );
}

END_METIL_NAMESPACE
