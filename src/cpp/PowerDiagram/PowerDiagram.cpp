#include "PowerDiagram.h"
#include "support/P.h"

PowerDiagram::PowerDiagram( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights ) {
    point_tree = PtPtr{ PointTree::New( cp, points, weights ) };
}

PowerDiagram::~PowerDiagram() {
}

void PowerDiagram::for_each_cell( const std::function<void( const Cell_2_double & )> &f ) {
    point_tree->for_each_leaf( [&]( Span<Point> points, Span<Scalar> weights, RemainingBoxes &remaining_boxes ) {
        P( points, weights );
    } );
}

DisplayItem *PowerDiagram::display( DisplayItemFactory &df ) const {
    return df.new_display_item( *point_tree );
}
