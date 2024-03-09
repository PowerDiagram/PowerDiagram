#include "PowerDiagram.h"
#include <mutex>

PowerDiagram::PowerDiagram( WeightedPointSet &&wps ) : wps( std::move( wps ) ) {
}

void PowerDiagram::set_weighted_point_set( WeightedPointSet &&wps ) {
    wps = std::move( wps );
}

void PowerDiagram::for_each_cell( const std::function<void( const Int &nb_threads, const std::function<void( const std::function<void( const Cell &cell, const Int &num_thread )> & )> & )> &f ) {
    VFS_CALL( for_each_cell_impl, CtStringList<"inc_file:PowerDiagram/impl/for_each_cell_impl.h">, void, wps, b_dirs, b_offs, f );
}

void PowerDiagram::for_each_cell( const std::function<void( const Cell & )> &f ) {
    for_each_cell( [&]( const Int &, const std::function<void( const std::function<void( const Cell &cell, const Int &num_thread )> & )> &cb ) {
        cb( [&]( const Cell &cell, const Int & ) {
            f( cell );
        } );
    } );
}

void PowerDiagram::display_vtk( const String &filename ) {
    for_each_cell( [&]( const Int &nb_threads, const auto &f ) {
        VtkOutput vo;
        std::mutex m;
        f( [&]( const Cell &cell, const Int &num_thread ) {
            const std::lock_guard<std::mutex> lock( m );
            cell.display_vtk( vo );
        });
        vo.save( filename );
    } );
}
