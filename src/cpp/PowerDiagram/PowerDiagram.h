#pragma once

#include "WeightedPointSet.h"
#include <vfs/String.h>
#include "Cell.h"

///
class PowerDiagram {
public:
    /***/            PowerDiagram          ( WeightedPointSet &&wps = make_WeightedPointSet_Empty() );

    void             set_weighted_point_set( WeightedPointSet &&wps );
    void             for_each_cell         ( const std::function<void( const Int &nb_threads, const std::function<void( const std::function<void( const Cell &cell, const Int &num_thread )> & )> & )> &f );
    void             for_each_cell         ( const std::function<void( const Cell &cell )> &f );

    void             display_vtk           ( const String &filename );

    List<Point>      b_dirs;               ///<
    Vector           b_offs;               ///<
    WeightedPointSet wps;                  ///<
};
