#pragma once

#include <tl/support/containers/operators/all.h>
#include <tl/support/containers/Pair.h>
#include "PowerDiagram.h"
#include <thread>

// #include "support/P.h"

#define DTP template<class Scalar,int nb_dims>
#define UTP PowerDiagram<Scalar,nb_dims>

DTP UTP::PowerDiagram( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, Span<Point> bnd_dirs, Span<Scalar> bnd_offs ) {
    point_tree = PtPtr{ PointTree<Scalar,nb_dims>::New( cp, points, weights, indices, nullptr, 0 ) };
    this->bnd_dirs = bnd_dirs;
    this->bnd_offs = bnd_offs;

    // limits
    min_box_pos = point_tree->min_point();
    max_box_pos = point_tree->max_point();
    if ( all( min_box_pos == max_box_pos ) && nb_dims )
        max_box_pos[ 0 ] += 1;

    // for( auto &v : min_box_pos )
    //     v = -1e7;
    // for( auto &v : max_box_pos )
    //     v = +1e7;

    // base cell
    base_cell.make_init_simplex( min_box_pos, max_box_pos );
    for( PI i = 0; i < bnd_offs.size(); ++i )
        base_cell.cut( bnd_dirs[ i ], bnd_offs[ i ], i );

    // base inf cell
    for( PI i = 0; i < bnd_offs.size(); ++i )
        base_inf_cell.cut( bnd_dirs[ i ], bnd_offs[ i ], i );
}

DTP void UTP::make_intersections( auto &cell, const RemainingBoxes<Scalar,nb_dims> &rb_base ) {
    const Scalar &w0 = *cell.orig_weight;
    const Point &p0 = *cell.orig_point;
    const PI i0 = cell.orig_index;

    const PI nb_bnds = bnd_offs.size();

    // helper
    auto cut = [&]( const Point &p1, const Scalar &w1, PI i1 ) {
        const Point dir = p1 - p0;
        auto n = norm_2_p2( dir );
        auto s0 = sp( dir, p0 );
        auto s1 = sp( dir, p1 );
        auto off = s0 + ( 1 + ( w0 - w1 ) / n ) / 2 * ( s1 - s0 );

        cell.cut( dir, off, nb_bnds + i1 );
    };

    // intersections with the points in the same box
    rb_base.leaf->for_each_point( [&]( const Point &p1, const Scalar &w1, const PI i1 ) {
        if ( i1 != i0 )
            cut( p1, w1, i1 );
    } );

    // helper to test if a bow may contain a dirac that can create a new cut
    const auto may_intersect = [&]( PointTree<Scalar,nb_dims> *point_tree ) -> bool {
        for( const Vertex<Scalar,nb_dims> &vertex : cell.vertices )
            if ( point_tree->may_intersect( vertex.pos, p0, w0 ) )
                return true;
        return false;
    };

    // intersections with the points other boxes that may create intersections
    for( RemainingBoxes<Scalar,nb_dims> rb = rb_base; rb.go_to_next_leaf( may_intersect ); ) {
        rb.leaf->for_each_point( [&]( const Point &p1, const Scalar &w1, const PI i1 ) {
            cut( p1, w1, i1 );
        } );
    }
}

DTP int UTP::max_nb_threads() {
    return std::thread::hardware_concurrency();
}

DTP void UTP::for_each_cell( const std::function<void( Cell<Scalar,nb_dims> & )> &f ) {
    std::mutex m;
    for_each_cell( [&]( Cell<Scalar,nb_dims> &cell, int ) {
        m.lock();
        f( cell );
        m.unlock();
    } );
}

DTP void UTP::for_each_cell( const std::function<void( Cell<Scalar,nb_dims> &, int )> &f ) {
    if ( ! point_tree )
        return;

    const PI nb_threads = max_nb_threads();
    Vec<PointTree<Scalar,nb_dims> *> leave_bounds = point_tree->split( nb_threads );

    Vec<std::thread> threads( FromSizeAndInitFunctionOnIndex(), nb_threads, [&]( std::thread *th, PI num_thread ) {
        new ( th ) std::thread( [&f,&leave_bounds,num_thread,this]() {
            Cell<Scalar,nb_dims> cell;
            for( PointTree<Scalar,nb_dims> *leaf = leave_bounds[ num_thread + 0 ]; leaf != leave_bounds[ num_thread + 1 ]; leaf = leaf->next_leaf() ) {
                leaf->for_each_point( [&]( const Point &p0, const Scalar &w0, const PI i0 ) {
                    // we may have to redo the cell if the base one is not large enough
                    while ( true ) {
                        // get a list of unexplored boxes
                        auto rb_base = RemainingBoxes<Scalar,nb_dims>::from_leaf( leaf );

                        // make a base cell
                        cell.init_geometry_from( base_cell );
                        cell.orig_weight = &w0;
                        cell.orig_point = &p0;
                        cell.orig_index = i0;

                        // make the cuts
                        make_intersections( cell, rb_base );

                        // if we missed a vertex because the base_cell is not large enough, restart with a new base_cell
                        bool inf_cut = cell.is_inf() && outside_cell( cell, rb_base );
                        if ( ! inf_cut ) {
                            f( cell, num_thread );
                            break;
                        }
                    }
                } );
            }
        } );
    } );

    for( std::thread &th : threads )
        th.join();
}

DTP void UTP::display_vtk( VtkOutput &vo ) {
    for_each_cell( [&]( const Cell<Scalar,nb_dims> &cell ) {
        cell.display_vtk( vo );
    } );
}

DTP Opt<std::tuple<const Scalar *, const typename UTP::Point *, SI>> UTP::cell_data_at( const Point &pt ) const {
    // inside ?
    for( PI i = 0; i < bnd_offs.size(); ++i )
        if ( sp( bnd_dirs[ i ], pt ) > bnd_offs[ i ] )
            return {};

    
    // TODO: optimize
    const Scalar *best_w0 = nullptr;
    const Point *best_p0 = nullptr;
    PI best_i0 = 0;
    Scalar best_v;
    for( RemainingBoxes<Scalar,nb_dims> rb_base = RemainingBoxes<Scalar,nb_dims>::for_first_leaf_of( point_tree.get() ); rb_base; rb_base.go_to_next_leaf() ) {
        for( PI n0 = 0, nc = rb_base.leaf->points.size(); n0 < nc; ++n0 ) {
            const Scalar &w0 = rb_base.leaf->weights[ n0 ];
            const Point &p0 = rb_base.leaf->points[ n0 ];
            const PI i0 = rb_base.leaf->indices[ n0 ];

            Scalar v = norm_2_p2( pt - p0 ) - w0;
            if ( ! best_w0 || best_v > v ) {
                best_w0 = &w0;
                best_p0 = &p0;
                best_i0 = i0;
                best_v = v;
            }
        }
    }

    if ( ! best_w0 )
        return {};
    return std::tuple<const Scalar *, const typename UTP::Point *, SI>{ best_w0, best_p0, best_i0 };
}

DTP Vec<std::tuple<const Scalar *, const typename UTP::Point *, SI>> UTP::cell_data_at( const Point &pt, Scalar probe_size ) const {
    // find the "best cell"
    auto cda = cell_data_at( pt );
    if ( ! cda )
        return {};

    //  
    Scalar best_v = norm_2_p2( pt - *std::get<1>( *cda ) ) - *std::get<0>( *cda );

    // find the cell within the ball 
    Vec<std::tuple<const Scalar *, const Point *, SI>> res;
    for( RemainingBoxes<Scalar,nb_dims> rb_base = RemainingBoxes<Scalar,nb_dims>::for_first_leaf_of( point_tree.get() ); rb_base; rb_base.go_to_next_leaf() ) {
        for( PI n0 = 0, nc = rb_base.leaf->points.size(); n0 < nc; ++n0 ) {
            const Scalar &w0 = rb_base.leaf->weights[ n0 ];
            const Point &p0 = rb_base.leaf->points[ n0 ];
            const PI i0 = rb_base.leaf->indices[ n0 ];

            Scalar v = norm_2_p2( pt - p0 ) - w0;
            if ( v - best_v <= probe_size )
                res << std::tuple<const Scalar *, const Point *, SI>{ &w0, &p0, i0 };
        }
    }

    return res;
}

#ifndef AVOID_DISPLAY
DTP void UTP::display( Displayer &df ) const {
    df << "pouet"; // df.new_display_item( *point_tree );
}
#endif

DTP Str UTP::type_name() {
    return "PowerDiagram";
}

DTP bool UTP::outside_cell( auto &cell, const RemainingBoxes<Scalar,nb_dims> &rb_base ) {
    // make the inf cell (i.e. without the inf bounds)
    InfCell<Scalar,nb_dims> inf_cell = base_inf_cell;
    make_intersections( inf_cell, rb_base );

    // check that the vertices of the inf_cell are inside the inf bounds
    bool has_outside_vertex = false;
    inf_cell.for_each_repr_point( [&]( const Point &pos ) {
        for( const Cut<Scalar,nb_dims> &cut : base_cell.cuts ) {
            if ( cut.is_inf() && ( sp( pos, cut.dir ) - cut.sp ) >= 0 ) {
                min_box_pos = min( min_box_pos, pos );
                max_box_pos = max( max_box_pos, pos );
                has_outside_vertex = true;
            }
        }
    } );

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
