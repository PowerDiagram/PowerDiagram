#pragma once

#include "support/operators/norm_2.h"
#include "support/operators/sp.h"
#include "PowerDiagram.h"

// #include "support/P.h"

#define DTP template<class Scalar,int nb_dims>
#define UTP PowerDiagram<Scalar,nb_dims>

DTP UTP::PowerDiagram( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, Span<Point> bnd_dirs, Span<Scalar> bnd_offs ) {
    point_tree = PtPtr{ PointTree<Scalar,nb_dims>::New( cp, points, weights, indices, nullptr ) };
    this->bnd_dirs = bnd_dirs;
    this->bnd_offs = bnd_offs;

    // limits
    min_box_pos = point_tree->min_point();
    max_box_pos = point_tree->max_point();
    if ( all( min_box_pos == max_box_pos ) && nb_dims )
        max_box_pos[ 0 ] += 1;

    // base cell
    base_cell.make_init_simplex( min_box_pos, max_box_pos );
    for( PI i = 0; i < bnd_offs.size(); ++i )
        base_cell.cut( bnd_dirs[ i ], bnd_offs[ i ], i );

    // base inf cell
    for( PI i = 0; i < bnd_offs.size(); ++i )
        base_inf_cell.cut( bnd_dirs[ i ], bnd_offs[ i ], i );
}

DTP void UTP::make_intersections( auto &cell, const RemainingBoxes<Scalar,nb_dims> &rb_base, PI n0 ) {
    const Scalar &w0 = rb_base.leaf->weights[ n0 ];
    const Point &p0 = rb_base.leaf->points[ n0 ];
    const PI nb_bnds = bnd_offs.size();

    // intersections with the points in the same box
    for( PI n1 = 0, nc = rb_base.leaf->points.size(); n1 < nc; ++n1 ) {
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
}

DTP void UTP::for_each_cell( const std::function<void( Cell<Scalar,nb_dims> & )> &f ) {
    if ( ! point_tree )
        return;

    // get the cell point
    Cell<Scalar,nb_dims> cell;
    for( RemainingBoxes<Scalar,nb_dims> rb_base = RemainingBoxes<Scalar,nb_dims>::for_first_leaf_of( point_tree.get() ); rb_base; rb_base.go_to_next_leaf() ) {
        for( PI n0 = 0, nc = rb_base.leaf->points.size(); n0 < nc; ++n0 ) {
            const Scalar &w0 = rb_base.leaf->weights[ n0 ];
            const Point &p0 = rb_base.leaf->points[ n0 ];
            const PI i0 = rb_base.leaf->indices[ n0 ];

            // we may have to redo the cell if the base one is not large enough
            while ( true ) {
                // start from the base cell
                cell.init_geometry_from( base_cell );
                cell.orig_weight = &w0;
                cell.orig_point = &p0;
                cell.orig_index = i0;

                // make the cuts
                make_intersections( cell, rb_base, n0 );

                // if we missed a vertex because the base_cell is not large enough, restart with a new base_cell
                bool inf_cut = cell.is_inf() && outside_cell( cell, rb_base, n0 );
                if ( ! inf_cut ) {
                    f( cell );
                    break;
                }
            }
        }
    }
}

#ifndef AVOID_DISPLAY
DTP DisplayItem *UTP::display( DisplayItemFactory &df ) const {
    return df.new_display_item( *point_tree );
}
#endif

DTP Str UTP::type_name() {
    return "PowerDiagram";
}

DTP bool UTP::outside_cell( auto &cell, const RemainingBoxes<Scalar,nb_dims> &rb_base, PI n0 ) {
    // make the inf cell (i.e. without the inf bounds)
    InfCell<Scalar,nb_dims> inf_cell = base_inf_cell;
    make_intersections( inf_cell, rb_base, n0 );

    // check that the vertices of the inf_cell are inside the inf bounds
    bool has_outside_vertex = false;
    for( const Vertex <Scalar,nb_dims> &vertex : inf_cell.vertices ) {
        for( const Cut<Scalar,nb_dims> &cut : base_cell.cuts ) {
            if ( cut.is_inf() && ( sp( vertex.pos, cut.dir ) - cut.sp ) > 0 ) {
                min_box_pos = min( min_box_pos, vertex.pos );
                max_box_pos = max( max_box_pos, vertex.pos );
                has_outside_vertex = true;
            }
        }
    }

    // update base cell if necessary
    if ( has_outside_vertex ) {
        base_cell.make_init_simplex( min_box_pos, max_box_pos );
        for( PI i = 0; i < bnd_offs.size(); ++i )
            base_cell.cut( bnd_dirs[ i ], bnd_offs[ i ], i );
    }

    return has_outside_vertex;
}

#undef DTP
#undef UTP
