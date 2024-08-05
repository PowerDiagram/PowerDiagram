#pragma once

#include "support/type_name.h"
#include "PointTree.h"

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class RemainingBoxes { STD_METIL_TYPE_INFO( RemainingBoxes, "", remaining_boxes, leaf )
public:
    static RemainingBoxes            for_first_leaf_of       ( PointTree<Scalar,nb_dims> *point_tree ); ///< take
    static RemainingBoxes            from_leaf               ( PointTree<Scalar,nb_dims> *leaf );

    RemainingBoxes&                  go_to_next_leaf         ( const std::function<bool( PointTree<Scalar,nb_dims> *point_tree )> &go_inside );
    RemainingBoxes&                  go_to_next_leaf         ();
    operator                         bool                    () const;

    static void                      from_leaf_rec           ( RemainingBoxes &res, PointTree<Scalar,nb_dims> *tree );

    Vec<PointTree<Scalar,nb_dims> *> remaining_boxes;
    PointTree<Scalar,nb_dims>*       leaf;
};

#include "RemainingBoxes.tcc"
