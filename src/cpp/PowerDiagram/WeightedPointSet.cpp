#include "WeightedPointSet.h"

WeightedPointSet make_WeightedPointSet_AABB( const List<Point> &positions, const Vector &weights ) {
    return VFS_CALL( make_WeightedPointSet_AABB_impl, CtStringList<"inc_file:PowerDiagram/impl/WeightedPointSet_AABB.h">, WeightedPointSet, positions, weights );
}

WeightedPointSet make_WeightedPointSet_Empty() {
    return VFS_CALL( make_WeightedPointSet_Empty, CtStringList<"inc_file:PowerDiagram/impl/WeightedPointSet_Empty.h">, WeightedPointSet );
}

Int WeightedPointSet::size() const {
    return VFS_CALL_METHOD( size, Int, *this );
}
