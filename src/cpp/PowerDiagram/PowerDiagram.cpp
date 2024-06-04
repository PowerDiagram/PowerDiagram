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

    // get the cell point
    Cell cell;
    for( RemainingBoxes rb_base = RemainingBoxes::for_first_leaf_of( point_tree.get() ); rb_base; rb_base.go_to_next_leaf() ) {
        for( PI n0 = 0, nc = rb_base.leaf->points.size(); n0 < nc; ++n0 ) {
            const Scalar &w0 = rb_base.leaf->weights[ n0 ];
            const Point &p0 = rb_base.leaf->points[ n0 ];
            const PI &i0 = rb_base.leaf->indices[ n0 ];
            cell.init( &p0, &w0, i0 );

            // intersections with the points in the same box
            for( PI n1 = 0; n1 < nc; ++n1 ) {
                if ( n1 == n0 )
                    continue;

                const Scalar &w1 = rb_base.leaf->weights[ n1 ];
                const Point &p1 = rb_base.leaf->points[ n1 ];
                const PI &i1 = rb_base.leaf->indices[ n1 ];

                const Point dir = p1 - p0;

                cell.init( &p0, &w0, i0 );
            }

            // other boxes
            const auto may_intersect = [&]( PointTree *point_tree ) -> bool { return true; };
            for( RemainingBoxes rb = rb_base; rb.go_to_next_leaf( may_intersect ); ) {
                for( PI n1 = 0; n1 < rb.leaf->points.size(); ++n1 ) {
                    TODO;
                }
            }
        }
    }

    // point_tree->for_each_leaf( [&]( Span<Point> points, Span<Scalar> weights, RemainingBoxes &remaining_boxes ) {
    //     P( points, weights );
    // } );
}

DisplayItem *PowerDiagram::display( DisplayItemFactory &df ) const {
    return df.new_display_item( *point_tree );
}
