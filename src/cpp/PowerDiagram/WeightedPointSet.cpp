#include "WeightedPointSet.h"
#include <vfs/Function.h>

WeightedPointSet make_WeightedPointSet_AABB( const List<Point> &positions, const Vector &weights ) {
    // Function f( "WeightedPointSet_AABB", { { "inc_file:PowerDiagram/impl/WeightedPointSet_AABB.h" } } );
    // return f.call<WeightedPointSet>( positions, weights );
    return VFS_CALL( WeightedPointSet_AABB, CtStringList<"inc_file:PowerDiagram/impl/WeightedPointSet_AABB.h">, WeightedPointSet, positions, weights );
}
