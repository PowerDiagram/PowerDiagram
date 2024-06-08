#pragma once

#include "support/operators/norm_2.h"
#include "support/operators/sp.h"
#include "RemainingBoxes.h"
#include "PowerDiagram.h"
// #include "support/P.h"

#define DTP template<class Scalar,int nb_dims>
#define UTP PowerDiagram<Scalar,nb_dims>

DTP UTP::PowerDiagram( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, Span<Point> bnd_dirs, Span<Scalar> bnd_offs ) {
    point_tree = PtPtr{ PointTree<Scalar,nb_dims>::New( cp, points, weights, indices, nullptr ) };
    this->bnd_dirs = bnd_dirs;
    this->bnd_offs = bnd_offs;
    coeff_init_simplex = 1e6;
}

DTP void UTP::for_each_cell( const std::function<void( Cell<Scalar,nb_dims> & )> &f ) {
    if ( ! point_tree )
        return;

    Point center = point_tree->center();
    auto radius = point_tree->radius();
    radius *= coeff_init_simplex;
    if ( ! radius )
        radius = 1e10; // TODO: something more robust

    // make the base cell
    Cell<Scalar,nb_dims> base_cell;
    base_cell.make_init_simplex( center, radius );

    // cut by the boundaries
    const PI nb_bnds = bnd_offs.size();
    for( PI i = 0; i < nb_bnds; ++i )
        base_cell.cut( bnd_dirs[ i ], bnd_offs[ i ], i );

    // get the cell point
    Cell<Scalar,nb_dims> cell;
    for( RemainingBoxes<Scalar,nb_dims> rb_base = RemainingBoxes<Scalar,nb_dims>::for_first_leaf_of( point_tree.get() ); rb_base; rb_base.go_to_next_leaf() ) {
        for( PI n0 = 0, nc = rb_base.leaf->points.size(); n0 < nc; ++n0 ) {
            const Scalar &w0 = rb_base.leaf->weights[ n0 ];
            const Point &p0 = rb_base.leaf->points[ n0 ];
            const PI &i0 = rb_base.leaf->indices[ n0 ];

            // start from the base cell
            cell.init_geometry_from( base_cell );
            cell.orig_weight = &w0;
            cell.orig_point = &p0;
            cell.orig_index = i0;

            // intersections with the points in the same box
            for( PI n1 = 0; n1 < nc; ++n1 ) {
                if ( n1 == n0 )
                    continue;

                const Scalar &w1 = rb_base.leaf->weights[ n1 ];
                const Point &p1 = rb_base.leaf->points[ n1 ];
                const PI &i1 = rb_base.leaf->indices[ n1 ];

                const Point dir = p1 - p0;
                auto n = norm_2_p2( dir );
                auto s0 = sp( dir, p0 );
                auto s1 = sp( dir, p1 );
                auto off = s0 + ( 1 + 1 * ( w0 - w1 ) / n ) / 2 * ( s1 - s0 );
                cell.cut( dir, off, nb_bnds + i1 );
            }

            // other boxes
            const auto may_intersect = [&]( PointTree<Scalar,nb_dims> *point_tree ) -> bool { return true; };
            for( RemainingBoxes<Scalar,nb_dims> rb = rb_base; rb.go_to_next_leaf( may_intersect ); ) {
                for( PI n1 = 0; n1 < rb.leaf->points.size(); ++n1 ) {
                    TODO;
                }
            }

            //
            f( cell );
        }
    }
}

DTP DisplayItem *UTP::display( DisplayItemFactory &df ) const {
    return df.new_display_item( *point_tree );
}

DTP Str UTP::type_name() {
    return "PowerDiagram";
}

#undef DTP
#undef UTP
