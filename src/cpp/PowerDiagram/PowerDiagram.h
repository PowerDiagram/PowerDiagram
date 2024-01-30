#pragma once

#include <vfs/Vector.h>
#include <vfs/Point.h>
#include <vfs/List.h>

TT using List = Vfs::List<T>;
using Vector = Vfs::Vector;
using Point = Vfs::Point;

///
class PowerDiagram {
public:
    /***/ PowerDiagram       ();

    void  set_pos_and_weights( const List<Point> &points, const Vector &weights );
};
