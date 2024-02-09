#include "PowerDiagram.h"

PowerDiagram::PowerDiagram( WeightedPointSet &&wps ) : wps( std::move( wps ) ) {
}

void PowerDiagram::set_weighted_point_set( WeightedPointSet &&wps ) {
    wps = std::move( wps );
}

void PowerDiagram::for_each_cell( const std::function<void( PI nb_threads, const std::function<void( const std::function<void( const Cell &cell, PI num_thread )> & )> & )> &f ) {
    VFS_CALL( for_each_cell_impl, CtStringList<"inc_file:PowerDiagram/impl/for_each_laguerre_cell_impl.h">, void, wps, f );
}

void PowerDiagram::for_each_cell( const std::function<void( const Cell & )> &f ) {
    for_each_cell( [&]( PI, const std::function<void( const std::function<void( const Cell &cell, PI num_thread )> & )> &cb ) {
        cb( [&]( const Cell &cell, PI ) {
            f( cell );
        } );
    } );
}
