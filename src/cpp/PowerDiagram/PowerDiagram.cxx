#pragma once

#include <tl/support/containers/Pair.h>
#include <tl/support/operators/all.h>
#include "PowerDiagram.h"
#include <thread>


// #include "support/P.h"

#define DTP template<class Config>
#define UTP PowerDiagram<Config>

DTP UTP::PowerDiagram( const PointTreeCtorParms &cp, Vec<Pt> &&points_, Vec<TF> &&weights_, Span<Pt> bnd_dirs, Span<TF> bnd_offs ) :
        bnd_dirs( bnd_dirs), bnd_offs( bnd_offs), weights( std::move( weights_ ) ), points( std::move( points_ ) ) {

    //
    indices.resize( points.size() );
    for( PI i = 0; i < points.size(); ++i )
        indices[ i ] = i;

    // make the point tree
    point_tree = PtPtr{ PointTree<Config>::New( cp, points, weights, indices, nullptr, 0 ) };

    // limits
    min_box_pos = point_tree->min_point();
    max_box_pos = point_tree->max_point();
    if ( nb_dims && all( min_box_pos == max_box_pos ) )
        max_box_pos[ 0 ] += 1;

    // base cell
    base_cell.init_geometry_to_encompass( min_box_pos, max_box_pos );
    for( PI i = 0; i < bnd_offs.size(); ++i )
        base_cell.cut_boundary( bnd_dirs[ i ], bnd_offs[ i ], i );
    base_cell.memory_compaction();
}

DTP void UTP::make_intersections( auto &cell, Vec<PI32> &buffer, const RemainingBoxes<Config> &rb_base, const CutInfo *prev_cuts ) {
    // // helper to test if a bow may contain a dirac that can create a new cut
    // const auto may_intersect = [&]( PointTree<Config> *point_tree ) -> bool {
    //     return point_tree->may_intersect( cell );
    // };

    //
    if ( prev_cuts ) {
        // previous intersections
        for( const auto &p : prev_cuts[ cell.i0 ] ) {
            p.first->for_each_point( [&]( const Pt &p1, TF w1, PI i1, PI32 n1 ) {
                cell.cut_dirac( p1, w1, i1, p.first, n1 );
            }, p.second );
        }

        // intersections with the points in the same box that have not been tested
        auto apc = prev_cuts[ cell.i0 ].find( rb_base.leaf );
        if ( apc != prev_cuts[ cell.i0 ].end() ) {
            rb_base.leaf->for_each_point( [&]( Span<Pt> p1s, Span<TF> w1s, Span<PI> i1s ) {
                for( PI n = 0; n < p1s.size(); ++n )
                    if ( i1s[ n ] != cell.i0 && ! apc->second.contains( n ) )
                        cell.cut_dirac( p1s[ n ], w1s[ n ], i1s[ n ], rb_base.leaf, n );
            } );
        }

        // intersections with the points other boxes that may create intersections and that have not been tested
        for( RemainingBoxes<Config> rb = rb_base; rb.go_to_next_leaf( cell ); ) {
            auto apc = prev_cuts[ cell.i0 ].find( rb.leaf );
            if ( apc != prev_cuts[ cell.i0 ].end() ) {
                rb.leaf->for_each_point( [&]( Span<Pt> p1s, Span<TF> w1s, Span<PI> i1s ) {
                    for( PI n = 0; n < p1s.size(); ++n )
                        if ( ! apc->second.contains( n ) )
                            cell.cut_dirac( p1s[ n ], w1s[ n ], i1s[ n ], rb.leaf, n );
                } );
            } else {
                rb.leaf->for_each_point( [&]( Span<Pt> p1s, Span<TF> w1s, Span<PI> i1s ) {
                    for( PI n = 0; n < p1s.size(); ++n )
                        cell.cut_dirac( p1s[ n ], w1s[ n ], i1s[ n ], rb.leaf, n );
                } );
            }
        }

        return;
    }

    // intersections with the points in the same box
    rb_base.leaf->for_each_point( [&]( Span<Pt> p1s, Span<TF> w1s, Span<PI> i1s ) {
        buffer.resize( p1s.size() - 1 );
        for( PI n = 0, j = 0; n < p1s.size(); ++n )
            if ( i1s[ n ] != cell.i0 )
                buffer[ j++ ] = n;
        std::sort( buffer.begin(), buffer.end(), [&]( PI32 a, PI32 b ) {
            return norm_2_p2( p1s[ a ] - cell.p0 ) < norm_2_p2( p1s[ b ] - cell.p0 );
        } );

        for( PI n : buffer )
            cell.cut_dirac( p1s[ n ], w1s[ n ], i1s[ n ], rb_base.leaf, n );
        // for( PI n = 0; n < p1s.size(); ++n )
        //     if ( i1s[ n ] != cell.i0 )
        //         cell.cut_dirac( p1s[ n ], w1s[ n ], i1s[ n ] );
    } );

    // intersections with the points other boxes that may create intersections
    for( RemainingBoxes<Config> rb = rb_base; rb.go_to_next_leaf( cell ); ) {
        rb.leaf->for_each_point( [&]( Span<Pt> p1s, Span<TF> w1s, Span<PI> i1s ) {
            buffer.resize( p1s.size() );
            for( PI n = 0; n < p1s.size(); ++n )
                buffer[ n ] = n;
            std::sort( buffer.begin(), buffer.end(), [&]( PI32 a, PI32 b ) {
                return norm_2_p2( p1s[ a ] - cell.p0 ) < norm_2_p2( p1s[ b ] - cell.p0 );
            } );

            for( PI n : buffer ) // for( PI n = 0; n < p1s.size(); ++n )
                cell.cut_dirac( p1s[ n ], w1s[ n ], i1s[ n ], rb.leaf, n );
            // for( PI n = 0; n < p1s.size(); ++n )
            //     cell.cut_dirac( p1s[ n ], w1s[ n ], i1s[ n ] );
        } );
    }
}

DTP int UTP::max_nb_threads() {
    return std::thread::hardware_concurrency();
}

DTP void UTP::for_each_cell( const std::function<void( Cell<Config> & )> &f ) {
    std::mutex m;
    for_each_cell( [&]( Cell<Config> &cell, int ) {
        m.lock();
        f( cell );
        m.unlock();
    } );
}

DTP void UTP::for_each_cell( const std::function<void( Cell<Config> &, int )> &f, const CutInfo *prev_cuts ) {
    if ( ! point_tree )
        return;

    const PI nb_threads = max_nb_threads();
    Vec<PointTree<Config> *> leave_bounds = point_tree->split( nb_threads );

    Vec<std::thread> threads( FromSizeAndInitFunctionOnIndex(), nb_threads, [&]( std::thread *th, PI num_thread ) {
        new ( th ) std::thread( [&f,&leave_bounds,num_thread,this,prev_cuts]() {
            Cell<Config> cell;
            Vec<PI32> buffer( FromReservationSize(), 32 );
            for( PointTree<Config> *leaf = leave_bounds[ num_thread + 0 ]; leaf != leave_bounds[ num_thread + 1 ]; leaf = leaf->next_leaf() ) {
                leaf->for_each_point( [&]( Span<Pt> p0s, Span<TF> w0s, Span<PI> i0s ) {
                    for( PI np = 0; np < p0s.size(); ++np ) {
                        // get a list of unexplored boxes
                        auto rb_base = RemainingBoxes<Config>::from_leaf( leaf );

                        // make a base cell
                        cell.init_geometry_from( base_cell );
                        cell.p0 = p0s[ np ];
                        cell.w0 = w0s[ np ];
                        cell.i0 = i0s[ np ];

                        // make the cuts
                        make_intersections( cell, buffer, rb_base, prev_cuts );

                        // if we missed a vertex because the base_cell is not large enough, restart with a new base_cell
                        f( cell, num_thread );
                    }
                } );
            }
        } );
    } );

    for( std::thread &th : threads )
        th.join();
}

DTP void UTP::display_vtk( VtkOutput &vo ) {
    for_each_cell( [&]( const Cell<Config> &cell ) {
        cell.display_vtk( vo );
    } );
}

DTP Opt<std::tuple<const typename UTP::TF *, const typename UTP::Pt *, SI>> UTP::cell_data_at( const Pt &pt ) const {
    // inside ?
    for( PI i = 0; i < bnd_offs.size(); ++i )
        if ( sp( bnd_dirs[ i ], pt ) > bnd_offs[ i ] )
            return {};

    
    // TODO: optimize
    const TF *best_w0 = nullptr;
    const Pt *best_p0 = nullptr;
    PI best_i0 = 0;
    TF best_v;
    for( RemainingBoxes<Config> rb_base = RemainingBoxes<Config>::for_first_leaf_of( point_tree.get() ); rb_base; rb_base.go_to_next_leaf() ) {
        for( PI n0 = 0, nc = rb_base.leaf->points.size(); n0 < nc; ++n0 ) {
            const TF &w0 = rb_base.leaf->weights[ n0 ];
            const Pt &p0 = rb_base.leaf->points[ n0 ];
            const PI i0 = rb_base.leaf->indices[ n0 ];

            TF v = norm_2_p2( pt - p0 ) - w0;
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
    return std::tuple<const TF *, const typename UTP::Pt *, SI>{ best_w0, best_p0, best_i0 };
}

DTP Vec<std::tuple<const typename UTP::TF *, const typename UTP::Pt *, SI>> UTP::cell_data_at( const Pt &pt, TF probe_size ) const {
    // find the "best cell"
    auto cda = cell_data_at( pt );
    if ( ! cda )
        return {};

    //  
    TF best_v = norm_2_p2( pt - *std::get<1>( *cda ) ) - *std::get<0>( *cda );

    // find the cell within the ball 
    Vec<std::tuple<const TF *, const Pt *, SI>> res;
    for( RemainingBoxes<Config> rb_base = RemainingBoxes<Config>::for_first_leaf_of( point_tree.get() ); rb_base; rb_base.go_to_next_leaf() ) {
        for( PI n0 = 0, nc = rb_base.leaf->points.size(); n0 < nc; ++n0 ) {
            const TF &w0 = rb_base.leaf->weights[ n0 ];
            const Pt &p0 = rb_base.leaf->points[ n0 ];
            const PI i0 = rb_base.leaf->indices[ n0 ];

            TF v = norm_2_p2( pt - p0 ) - w0;
            if ( v - best_v <= probe_size )
                res << std::tuple<const TF *, const Pt *, SI>{ &w0, &p0, i0 };
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

DTP bool UTP::outside_cell( auto &cell, const RemainingBoxes<Config> &rb_base ) {
    return false;
    // // make the inf cell (i.e. without the inf bounds)
    // InfCell<Config> inf_cell = base_inf_cell;
    // inf_cell.w0 = cell.w0;
    // inf_cell.p0 = cell.p0;
    // inf_cell.i0 = cell.i0;

    // make_intersections( inf_cell, rb_base );

    // // check that the vertices of the inf_cell are inside the inf bounds
    // bool has_outside_vertex = false;
    // inf_cell.for_each_repr_point( [&]( const Point &pos ) {
    //     for( const CellCut<TF,nb_dims> &cut : base_cell.cuts ) {
    //         if ( cut.is_inf() && ( sp( pos, cut.dir ) - cut.off ) >= 0 ) {
    //             min_box_pos = min( min_box_pos, pos );
    //             max_box_pos = max( max_box_pos, pos );
    //             has_outside_vertex = true;
    //         }
    //     }
    // } );

    // // update base cell if necessary
    // if ( has_outside_vertex ) {
    //     base_cell.make_init_simplex( min_box_pos, max_box_pos );
    //     for( PI i = 0; i < bnd_offs.size(); ++i )
    //         base_cell.cut_boundary( bnd_dirs[ i ], bnd_offs[ i ], i );
    // }

    // return has_outside_vertex;
}

#undef DTP
#undef UTP
