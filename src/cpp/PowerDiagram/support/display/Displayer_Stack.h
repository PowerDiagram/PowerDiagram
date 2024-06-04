#pragma once

#include "../BumpPointerPool.h"
#include "DisplayerItem.h"
#include "DisplayItemFactory.h"
#include <functional>
#include <vector>
#include <string>
#include <map>

BEG_METIL_NAMESPACE

/**
*/
class Displayer_Stack : public DisplayItemFactory {
public:
    using           ItemMap          = std::map<std::string,DisplayerItem *>;
    using           ItemStack        = std::vector<DisplayerItem *>;

    /**/            Displayer_Stack  ();

    virtual bool    start_string_item( std::string_view id = {} ) override;
    virtual bool    start_number_item( std::string_view id = {} ) override;
    virtual bool    start_array_item ( std::string_view id = {} ) override;

    // end of items
    virtual void    end_item         () override;

    // generic attributes for the next item
    virtual void    set_next_position( std::string_view file, unsigned line, unsigned col ) override;
    virtual void    set_next_type    ( std::string_view type ) override;
    virtual void    set_next_name    ( std::string_view name ) override;

    // string attributes
    virtual void    set_string_content      ( std::string_view content ) override; ///< works only for strings

    // number attributes
    virtual void    set_denominator  ( std::string_view denominator ) override; ///< works only for numbers
    virtual void    set_numerator    ( std::string_view numerator ) override; ///< works only for numbers
    virtual void    set_precision    ( std::string_view precision ) override; ///< works only for numbers
    virtual void    set_exponant     ( std::string_view exponant ) override; ///< works only for numbers

    //
    virtual void    on_end_item      ( DisplayerItem *item, bool partial ) = 0;

private:
    bool            place_new_item   ( std::string_view id, const std::function<DisplayerItem *()> &creator );

    std::string     next_position;
    std::string     next_type;
    std::string     next_name;
    std::string     next_file;
    int             next_line;
    int             next_col;

    DisplayerItem*  current_item;
    ItemStack       item_stack;
    ItemMap         item_map;
    BumpPointerPool pool;
};

END_METIL_NAMESPACE
