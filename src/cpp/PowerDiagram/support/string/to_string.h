#pragma once

#include "../display/DisplayWriteContext.h"
#include "../display/DisplayItemFactory.h"
#include "../display/DisplayItem.h"
#include "../common_macros.h"
#include <sstream>

BEG_METIL_NAMESPACE

TT std::string to_string( T &&val ) {
    // create a root display item
    DisplayItemFactory ds;
    DisplayItem *item = ds.new_display_item( val );

    // make a string
    std::ostringstream ss;
    DisplayWriteContext ctx;
    auto writer = [&]( std::string_view str ) { ss.write( str.data(), str.size() ); };
    item->write( writer, ctx );

    return ss.str();
}

END_METIL_NAMESPACE
