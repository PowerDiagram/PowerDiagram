#pragma once

#include "DisplayTypeInfo.h"
#include <string_view>
#include <functional>
#include <chrono>
#include <string>

BEG_METIL_NAMESPACE
class DisplayWriteContext;
class DisplayShowContext;

/***/
class DisplayItem {
public:
    struct          SrcInfo      { std::string filename; int line, col; };
    using           Date         = std::chrono::time_point<std::chrono::high_resolution_clock>;

    /**/            DisplayItem  ( DisplayTypeInfo ti );
    virtual        ~DisplayItem  ();

    virtual bool    need_cr      ( DisplayWriteContext &ctx ) const = 0;
    virtual void    write        ( const std::function<void( std::string_view str )> &func, DisplayWriteContext &ctx ) const = 0;
    virtual void    show         ( DisplayShowContext &ctx ) const = 0;

    DisplayTypeInfo type_info;   ///<
    SrcInfo*        src_info;    ///<
    PI              nb_use;      ///<
    std::string     name;        ///<

    DisplayItem*    prev;        ///<
    DisplayItem*    next;        ///<
};

END_METIL_NAMESPACE
