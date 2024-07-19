#pragma once

#include "display/DisplayWriteContext.h"
#include "display/DisplayItemFactory.h"
#include "display/DisplayItem.h"
#include <iostream>
#include <mutex>

BEG_METIL_NAMESPACE
template<class Value> std::string type_name();

void __print_with_mutex( std::ostream &os, DisplayWriteContext &&ctx, std::string_view arg_names, const auto &...arg_values ) {
    // create a root display item
    DisplayItemFactory ds;
    DisplayItem *item = ds.new_object( {}, [&]( DisplayObjectFiller &dof ) {
        dof.add( arg_names, arg_values... );
    } );

    // make a string
    std::ostringstream ss;
    auto writer = [&]( std::string_view str ) { ss.write( str.data(), str.size() ); };
    item->write( writer, ctx );
    ds.write_pointers( writer, ctx );

    // display it
    static std::mutex m;
    m.lock();    
    os << ss.str() << std::endl;
    m.unlock();
}

// template<class OS,class... ArgValues>
// void __print_types_with_mutex( OS &os, const DisplayParameters &dp, std::string_view arg_names, ArgValues &&...arg_values ) {
//     __print_with_mutex( os, dp, arg_names, type_name<decltype(arg_values)>()... );
// }

template<class... ArgValues>
void __show( const Str &arg_names, ArgValues &&...arg_values ) {
    // create a root display item
    DisplayItemFactory ds;
    DisplayItem *item = ds.new_object( {}, [&]( DisplayObjectFiller &dof ) {
        dof.add( arg_names, arg_values... );
    } );

    ds.show( item );
}

#ifndef P
    // PRINT in cout
    #define P( ... ) \
        METIL_NAMESPACE::__print_with_mutex( std::cout, DisplayWriteContext(), #__VA_ARGS__, __VA_ARGS__ )

    // PRINT in cerr
    #define PE( ... ) \
        METIL_NAMESPACE::__print_with_mutex( std::cerr, METIL_NAMESPACE::DisplayParameters::for_debug_info(), #__VA_ARGS__, __VA_ARGS__ )

    // PRINT in cout with options
    #define PO( CTX, ... ) \
        METIL_NAMESPACE::__print_with_mutex( std::cout, CTX, #__VA_ARGS__, __VA_ARGS__ )

    // PRINT in cout
    #define PT( ... ) \
        METIL_NAMESPACE::__print_types_with_mutex( std::cout, METIL_NAMESPACE::DisplayParameters::for_debug_info(), #__VA_ARGS__, __VA_ARGS__ )

    // PRINT with .display in cout with options
    #define PD( VALUE, ... ) \
        ( VALUE ).display( __VA_ARGS__ ).display( std::cout  << #VALUE " -> \n" ) << std::endl

    // PRINT with file and line info
    #define PM( ... ) \
        __print_with_mutex( std::cout << __FILE__ << ':' << __LINE__, " -> ", ", ", {}, #__VA_ARGS__, __VA_ARGS__, WithSep{""},  )

    // Display graph
    #define SHOW( ... ) \
        METIL_NAMESPACE::__show( #__VA_ARGS__, __VA_ARGS__ )

    // PRINT counter
    #define PC do { static int cpt = 0; PE( cpt++ ); } while ( false )
#endif

END_METIL_NAMESPACE
