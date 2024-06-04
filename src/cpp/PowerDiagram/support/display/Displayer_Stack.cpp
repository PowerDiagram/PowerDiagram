#include "DisplayerItem_String.h"
#include "DisplayerItem_Number.h"
#include "DisplayerItem_Array.h"
#include "Displayer_Stack.h"
#include "../TODO.h"

BEG_METIL_NAMESPACE

Displayer_Stack::Displayer_Stack() : current_item( nullptr ) {
}

bool Displayer_Stack::start_string_item( std::string_view id ) {
    return place_new_item( id, [&]() { return pool.create<DisplayerItem_String>(); } );
}

bool Displayer_Stack::start_number_item( std::string_view id ) {
    return place_new_item( id, [&]() { return pool.create<DisplayerItem_Number>(); } );
}

bool Displayer_Stack::start_array_item( std::string_view id ) {
    return place_new_item( id, [&]() { return pool.create<DisplayerItem_Array>(); } );
}

void Displayer_Stack::end_item() {
    //
    const bool partial = ! item_stack.empty();
    on_end_item( current_item, partial );

    //
    if ( partial ) {
        current_item = item_stack.back();
        item_stack.pop_back();
    } else
        current_item = nullptr;
}

void Displayer_Stack::set_next_position( std::string_view file, unsigned int line, unsigned int col ) {
    next_file = file;
    next_line = line;
    next_col = col;
}

void Displayer_Stack::set_next_type( std::string_view type ) {
    next_type = type;
}

void Displayer_Stack::set_next_name( std::string_view name ) {
    next_name = name;
}

void Displayer_Stack::set_string_content( std::string_view content ) {
    current_item->set_content( content );
}

void Displayer_Stack::set_denominator( std::string_view denominator ) {
}

void Displayer_Stack::set_numerator( std::string_view numerator ) {
    current_item->set_numerator( numerator );
}

void Displayer_Stack::set_precision( std::string_view precision ) {
}

void Displayer_Stack::set_exponant( std::string_view exponant ) {
}

bool Displayer_Stack::place_new_item( std::string_view id, const std::function<DisplayerItem *()> &creator ) {
    // no id ?
    if ( id.empty() ) {
        DisplayerItem *item = creator();
        if ( current_item ) {
            item_stack.push_back( current_item );
            current_item->add_item( item );
        }
        current_item = item;
    } else {
        //
        TODO;
        // auto iter = item_map.find( std::string( id ) );
        // if ( iter != item_map.end() ) {
        //     iter = item_map.insert( iter, { std::string( id ), new DisplayerItem_String } );
        //     return false;
        // }
    }

    current_item->position = next_position;
    current_item->type = next_type;
    current_item->name = next_name;
    current_item->file = next_file;
    current_item->line = next_line;
    current_item->col = next_col;

    next_position.clear();
    next_type.clear();
    next_name.clear();
    next_file.clear();
    next_line = -1;
    next_col = -1;

    return true;
}

END_METIL_NAMESPACE
