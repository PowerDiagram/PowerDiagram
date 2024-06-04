#include "RemainingBoxes.h"
#include "PowerDiagram.h"
#include "support/P.h"

PowerDiagram::PowerDiagram( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices ) {
    point_tree = PtPtr{ PointTree::New( cp, points, weights, indices, nullptr ) };
}

PowerDiagram::~PowerDiagram() {
}

void PowerDiagram::for_each_cell( const std::function<void( const Cell_2_double & )> &f ) {
    if ( ! point_tree )
        return;

    Cell cell;
    for( RemainingBoxes rb_base = RemainingBoxes::for_first_leaf_of( point_tree.get() ); rb_base; rb_base.go_to_next_leaf() ) {
        // cell.orig_point = ;
        P( rb_base.leaf );
    }

    // point_tree->for_each_leaf( [&]( Span<Point> points, Span<Scalar> weights, RemainingBoxes &remaining_boxes ) {
    //     P( points, weights );
    // } );
}

DisplayItem *PowerDiagram::display( DisplayItemFactory &df ) const {
    return df.new_display_item( *point_tree );
}
