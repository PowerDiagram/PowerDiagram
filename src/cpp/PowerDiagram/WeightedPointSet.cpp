#include "WeightedPointSet.h"
#include <vfs/Function.h>

WeightedPointSet make_WeightedPointSet_AABB( const List<Point> &positions, const Vector &weights ) {
    Function f( "WeightedPointSet_AABB", { { "inc_name:PowerDiagram/impl/WeightedPointSet_AABB.h" } } );
    return f.call<WeightedPointSet>( positions, weights );
}
