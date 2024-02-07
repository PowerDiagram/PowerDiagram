#pragma once

#include <vfs/Vector.h>
#include <vfs/Point.h>
#include <vfs/List.h>


/// wrapper around WeightedPointSet specializations
class WeightedPointSet { VFS_TD_ATTRIBUTES( WeightedPointSet, "PowerDiagram" );
public:
};

/// axis aligned bounding boxes
WeightedPointSet make_WeightedPointSet_AABB( const List<Point> &positions, const Vector &weights );
