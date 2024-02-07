#pragma once

#include "WeightedPointSet.h"
#include <vfs/Vector.h>
#include <vfs/Point.h>
#include <vfs/List.h>

///
class PowerDiagram {
public:
    /***/ PowerDiagram       ();

    void  set_pos_and_weights( const List<Point> &positions, const Vector &weights );



};
