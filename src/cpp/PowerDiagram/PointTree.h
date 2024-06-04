#pragma once

#include "support/display/DisplayItemFactory.h"
#include "support/CC_DT.h"

#define PointTree CC_DT( PointTree )

/**
*/
class PointTree {
public:
    virtual             ~PointTree();

    virtual DisplayItem *display  ( DisplayItemFactory &df ) const = 0;
};
