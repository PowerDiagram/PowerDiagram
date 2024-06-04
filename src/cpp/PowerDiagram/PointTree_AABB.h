#pragma once

#include "PointTree.h"

#define PointTree_AABB CC_DT( PointTree_AABB )

/**
*/
class PointTree_AABB : public PointTree {
public:
    /**/                 PointTree_AABB();

    virtual DisplayItem *display  ( DisplayItemFactory &df ) const;
};
