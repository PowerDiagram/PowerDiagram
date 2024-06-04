#include "RemainingBoxes.h"
#include "PointTree.h"

RemainingBoxes RemainingBoxes::for_first_leaf_of( PointTree *point_tree ) {
    RemainingBoxes res;

    if ( point_tree ) {
        while ( const PI nc = point_tree->children.size() ) {
            for( PI i = 1; i < nc; ++i )
                res.remaining_boxes << point_tree->children[ i ].get();
            point_tree = point_tree->children[ 0 ].get();
        }

        res.leaf = point_tree;
    } else {
        res.leaf = nullptr;
    }

    return res;
}

void RemainingBoxes::go_to_next_leaf( const std::function<bool( PointTree *point_tree )> &go_inside ) {
    // early return if found a leaf
    while ( remaining_boxes.size() ) {
        auto *point_tree = remaining_boxes.pop_back_val();
        if ( ! go_inside( point_tree ) )
            continue;

        if ( point_tree->children.empty() ) {
            leaf = point_tree;
            return;
        }


        for( PI i = 0, nc = point_tree->children.size(); i < nc; ++i ) {
            leaf = point_tree->children[ i ].get();
            if ( go_inside( leaf ) ) {
                for( PI j = i + 1; j < nc; ++j )
                    remaining_boxes << point_tree->children[ j ].get();

                if ( leaf->children.empty() )
                    return;

            }
            for( PI i = 1; i < nc; ++i )
                res.remaining_boxes << point_tree->children[ i ].get();
            point_tree = point_tree->children[ 0 ].get();
        }

        res.leaf = point_tree;

        if ( ! go_inside( b ) )
            continue;

        push_first_leaf_of( b );
    }

    //
    leaf = nullptr;
}

RemainingBoxes::operator bool() const {
    return leaf;
}
