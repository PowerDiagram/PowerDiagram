#include <PowerDiagram/PowerDiagram.h>
#include <PowerDiagram/VtkOutput.h>
#include "catch_main.h"

// template<typename compilation_flags> void f() {
//     compilation_flags::for_each_string( []( auto s ) { P( s ); } );
// }
// template<CtStringList compilation_flags> void f() {
//     compilation_flags.for_each_string( []( auto s ) { P( s ); } );
// }

TEST_CASE( "PowerDiagram", "" ) {
    List<Point> positions{ { 0.0, 0.0 }, { 1.0, 0.0 } };
    Vector weights{ 0, 0 };

    WeightedPointSet wps = make_WeightedPointSet_AABB( positions, weights );
    PowerDiagram pd( std::move( wps ) );

    VtkOutput vo;
    pd.for_each_cell( [&]( int nb_threads, const auto &f ) {
        f( [&]( const Cell &cell, int num_thread ) {
            //cell.display_vtk( vo );
            P( cell );
        });
    } );
    vo.save( "test.vtk" );


    // const int dim = 2;
    // const int nb_points = 10;
    // Vector v = Vector::randu( dim * nb_points, 0.0, 1.0 );
    // auto points = List<Point>::from_size_and_external_item_values( nb_points, { .item_type = Point::type_for( dim ) }, v.data() );
    // auto weights = Vector::ones( nb_points );

    // WeightedPointSet wps = make_WeightedPointSet_PolytopRec( points, weights );
    // PowerDiagram pd( &wps );


    // P( pd.volumes() );
}
