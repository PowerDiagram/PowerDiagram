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

RemainingBoxes &RemainingBoxes::go_to_next_leaf( const std::function<bool( PointTree *point_tree )> &go_inside ) {
    // we're looking for an leaf (validated by `go_inside`)
    while ( remaining_boxes.size() ) {
        // test if we have to go inside into the last remaining box
        auto *point_tree = remaining_boxes.pop_back_val();
        if ( ! go_inside( point_tree ) )
            continue;

        // if it's a leaf, it's done
        if ( point_tree->children.empty() ) {
            leaf = point_tree;
            return *this;
        }

        // else, push children in remaining boxes
        for( PI i = 0, nc = point_tree->children.size(); i < nc; ++i ) {
            PointTree *trial = point_tree->children[ i ].get();
            if ( go_inside( trial ) ) {
                for( PI j = nc; j-- > i; )
                    remaining_boxes << point_tree->children[ j ].get();
                break;
            }
        }
    }

    // not found
    leaf = nullptr;
    return *this;
}

RemainingBoxes &RemainingBoxes::go_to_next_leaf() {
    return go_to_next_leaf( []( PointTree * ) { return true; } );
}

RemainingBoxes::operator bool() const {
    return leaf;
}
