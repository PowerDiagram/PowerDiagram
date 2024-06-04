#include "PointTree_AABB.h"

PointTree_AABB::PointTree_AABB() {
}

DisplayItem *PointTree::display( DisplayItemFactory &df ) const {
    return df.new_string( CtType<PointTree>(), "yo" );
}
