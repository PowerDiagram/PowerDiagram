#pragma once

#include <tl/support/type_info/type_name.h>
#include "PointTree.h"

/**
 * @brief 
 * 
 */
template<class Config>
class RemainingBoxes { STD_TL_TYPE_INFO( RemainingBoxes, "", remaining_boxes, leaf )
public:
    static RemainingBoxes     for_first_leaf_of       ( PointTree<Config> *point_tree ); ///< take
    static RemainingBoxes     from_leaf               ( PointTree<Config> *leaf );

    // RemainingBoxes&        go_to_next_leaf         ( const std::function<bool( PointTree<Config> *point_tree )> &go_inside );
    RemainingBoxes&           go_to_next_leaf         ( const Cell<Config> &cell );
    RemainingBoxes&           go_to_next_leaf         ();
    void                      sort_by_dist            ( const auto &pos );
    operator                  bool                    () const;

    static void               from_leaf_rec           ( RemainingBoxes &res, PointTree<Config> *tree );

    Vec<PointTree<Config> *>  remaining_boxes;
    PointTree<Config>*        leaf;
};

#include "RemainingBoxes.tcc"
