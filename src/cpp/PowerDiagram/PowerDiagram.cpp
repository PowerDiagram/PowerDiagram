#include "PowerDiagram.h"

PowerDiagram::PowerDiagram( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights ) {
    point_tree = PtPtr{ PointTree::New( cp, points, weights ) };
}

PowerDiagram::~PowerDiagram() {
}

DisplayItem *PowerDiagram::display( DisplayItemFactory &df ) const {
    return df.new_display_item( *point_tree );
}
