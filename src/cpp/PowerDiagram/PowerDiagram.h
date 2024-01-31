#pragma once

#include "WeightedPointSet.h"
#include <vfs/Vector.h>
#include <vfs/Point.h>
#include <vfs/List.h>

///
class PowerDiagram {
public:
    /***/ PowerDiagram       ( const List<Point> &pos, const Vector &weights );

    void  set_pos_and_weights( const List<Point> &pos, const Vector &weights );



};
