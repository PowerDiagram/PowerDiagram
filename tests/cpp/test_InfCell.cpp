#include "sdot/InfCell.h"
#include "catch_main.h"

TEST_CASE( "PowerDiagram", "" ) {
    constexpr int nb_dims = 2;
    using Scalar = double;
    using Point = Vec<Scalar,nb_dims>;

    InfCell<Scalar,nb_dims> c;

    c.cut( { 1, 0 }, 1, 10 );
    P( c );

    c.cut( { 1, 0 }, 2, 11 );
    P( c );

    c.cut( { 1, 0 }, 0.5, 12 );
    P( c );

    // VtkOutput vo;
    // pd.for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell ) {
    //     cell.display_vtk( vo );
    //     // P( cell );
    // } );

    // vo.save( "pd.vtk" );
}
