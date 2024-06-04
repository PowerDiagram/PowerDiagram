#pragma once

#include "PointTree.h"

#define RemainingBoxes CC_DT( RemainingBoxes )

/**
 * @brief 
 * 
 */
class RemainingBoxes { STD_METIL_TYPE_INFO( RemainingBoxes, "", remaining_boxes )
public:
    static RemainingBoxes for_first_leaf_of( PointTree *point_tree );

    void                  go_to_next_leaf  ( const std::function<bool( PointTree *point_tree )> &go_inside );
    void                  go_to_next_leaf  ();
    operator              bool             () const;

    Vec<PointTree *>      remaining_boxes;
    PointTree*            leaf;
};
