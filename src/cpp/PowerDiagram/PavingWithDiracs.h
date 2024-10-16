#pragma once

#include <tl/support/Displayer.h>
#include "Cell.h"


/**
 * @brief Abstract class to describe a set of point in connected sub-regions
 * 
 */
class PD_NAME( PavingWithDiracs ) { STD_TL_TYPE_INFO( PavingWithDiracs, "", yo )
public:
    void for_each_cell( const std::function<void( Cell )> &f );
    //void display(  ) 
    int yo = 17;
};
