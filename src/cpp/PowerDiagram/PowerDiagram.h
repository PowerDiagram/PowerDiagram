#pragma once

#include "WeightedPointSet.h"
#include "Cell.h"

///
class PowerDiagram {
public:
    /***/            PowerDiagram          ( WeightedPointSet &&wps = make_WeightedPointSet_Empty() );

    void             set_weighted_point_set( WeightedPointSet &&wps );
    void             for_each_cell         ( const std::function<void( PI nb_threads, const std::function<void( const std::function<void( const Cell &cell, PI num_thread )> & )> & )> &f );
    void             for_each_cell         ( const std::function<void( const Cell &cell )> &f );

    WeightedPointSet wps;                  ///<
};
