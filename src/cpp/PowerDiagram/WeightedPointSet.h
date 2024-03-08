#pragma once

#include <vfs/Vector.h>
#include <vfs/Point.h>
#include <vfs/List.h>


/// wrapper around WeightedPointSet specializations
class WeightedPointSet {
public:
    VfsDtObject_STD_METHODS( WeightedPointSet, "PowerDiagram", 3 * sizeof( void * ), alignof( void * ) );

    Int size() const;
};

/// axis aligned bounding boxes
WeightedPointSet make_WeightedPointSet_AABB( const List<Point> &positions, const Vector &weights );

///
WeightedPointSet make_WeightedPointSet_Empty();
