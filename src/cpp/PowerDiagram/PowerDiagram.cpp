#include "support/operators/norm_2.h"
#include "support/operators/sp.h"
#include "support/P.h"
#include "RemainingBoxes.h"
#include "PowerDiagram.h"

PowerDiagram::PowerDiagram( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices ) {
    point_tree = PtPtr{ PointTree::New( cp, points, weights, indices, nullptr ) };
    coeff_init_simplex = 1;
}

PowerDiagram::~PowerDiagram() {
}

void PowerDiagram::for_each_cell( const std::function<void( const Cell & )> &f ) {
    if ( ! point_tree )
        return;

    Point center = point_tree->center();
    auto radius = point_tree->radius();
    if ( ! radius )
        radius = 1e10; // TODO: something more robust

    // get the cell point
    Cell cell;
    for( RemainingBoxes rb_base = RemainingBoxes::for_first_leaf_of( point_tree.get() ); rb_base; rb_base.go_to_next_leaf() ) {
        for( PI n0 = 0, nc = rb_base.leaf->points.size(); n0 < nc; ++n0 ) {
            const Scalar &w0 = rb_base.leaf->weights[ n0 ];
            const Point &p0 = rb_base.leaf->points[ n0 ];
            const PI &i0 = rb_base.leaf->indices[ n0 ];
            cell.init( &p0, &w0, i0, center, radius );

            // // intersections with the points in the same box
            // for( PI n1 = 0; n1 < nc; ++n1 ) {
            //     if ( n1 == n0 )
            //         continue;

            //     const Scalar &w1 = rb_base.leaf->weights[ n1 ];
            //     const Point &p1 = rb_base.leaf->points[ n1 ];
            //     const PI &i1 = rb_base.leaf->indices[ n1 ];

            //     const Point dir = p1 - p0;
            //     auto n = norm_2_p2( dir );
            //     auto s0 = sp( dir, p0 );
            //     auto s1 = sp( dir, p1 );
            //     auto off = s0 + ( 1 + 1 * ( w0 - w1 ) / n ) / 2 * ( s1 - s0 );
            //     cell.cut( dir, off, i1 );
            // }

            // // other boxes
            // const auto may_intersect = [&]( PointTree *point_tree ) -> bool { return true; };
            // for( RemainingBoxes rb = rb_base; rb.go_to_next_leaf( may_intersect ); ) {
            //     for( PI n1 = 0; n1 < rb.leaf->points.size(); ++n1 ) {
            //         TODO;
            //     }
            // }

            //
            f( cell );
        }
    }
}

DisplayItem *PowerDiagram::display( DisplayItemFactory &df ) const {
    return df.new_display_item( *point_tree );
}
