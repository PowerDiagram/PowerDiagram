#pragma once

#include <tl/support/containers/operators/determinant.h>
#include <tl/support/containers/operators/lu_solve.h>
#include <tl/support/containers/operators/norm_2.h>
#include <tl/support/containers/operators/sp.h>
#include <tl/support/containers/Opt.h>
#include <tl/support/compare.h>
#include <tl/support/conv.h>
#include "Cell.h"

#include <tl/support/P.h>
 
// #include <eigen3/Eigen/LU>

#define DTP template<class Scalar,int nb_dims>
#define UTP Cell<Scalar,nb_dims>

DTP UTP::Cell() {
    vertex_coords.reserve( 128 );
    vertex_cuts.reserve( 128 );
    sps.reserve( 128 );
    
    otps.reserve( 32 );

    last_inactive_cut = -1;
    last_active_cut = -1;
    curr_op_id = 0;
}

DTP void UTP::init_geometry_from( const Cell &that ) {
    last_inactive_cut = that.last_inactive_cut;
    last_active_cut = that.last_active_cut;
    vertex_coords = that.vertex_coords;
    vertex_cuts = that.vertex_cuts;
    cuts = that.cuts;
}

DTP void UTP::make_init_simplex( const Point &mi, const Point &ma ) {
    Point min_pos = ( mi + ma ) / 2 - ( ma - mi );
    Point max_pos = ( mi + ma ) / 2 + ( ma - mi );

    vertex_coords.clear();
    vertex_cuts.clear();
    cuts.clear();

    // cuts
    for( int d = 0; d < nb_dims; ++d ) {
        Point dir( FromItemValue(), 0 );
        dir[ d ] = -1;

        auto *cut = cuts.push_back();
        cut->type = CutType::Infinity;
        cut->prev = d - 1;
        cut->dir = dir;
        cut->off = sp( min_pos, dir );
        cut->i1 = d;
    }

    auto *cut = cuts.push_back();
    cut->type = CutType::Infinity;
    cut->prev = nb_dims - 1;
    cut->dir = { FromItemValue(), 1 };
    cut->off = sp( max_pos, cut->dir );
    cut->i1 = nb_dims;

    // last_active_cut
    last_active_cut = nb_dims;

    // vertices
    for( int nc_0 = 0; nc_0 < nb_dims + 1; ++nc_0 ) {
        Vec<PI,nb_dims> num_cuts( FromItemValue(), 0 );
        for( int i = 0; i < nc_0; ++i )
            num_cuts[ i ] = i;
        for( int i = nc_0 + 1; i < nb_dims + 1; ++i )
            num_cuts[ i - 1 ] = i;

        vertex_coords << compute_pos( num_cuts );
        vertex_cuts << num_cuts;
    }
}

DTP void UTP::apply_corr( auto &v0, auto &v1, Vec<int> &keep ) {
    int last_keep = v0.size();
    for( int i = 0; i < last_keep; ++i ) {
        if ( keep[ i ] ) {
            keep[ i ] = i;
            continue;
        }

        while( --last_keep > i && ! keep[ last_keep ] )
            keep[ last_keep ] = -1;

        v0.set_item( i, std::move( v0[ last_keep ] ) );
        v1.set_item( i, std::move( v1[ last_keep ] ) );
        keep[ last_keep ] = i;
        keep[ i ] = -1;
    }

    v0.resize( last_keep );
    v1.resize( last_keep );
}

DTP void UTP::apply_corr( Vec<int> &keep, auto &vec ) {
    int last_keep = vec.size();
    for( int i = 0; i < last_keep; ++i ) {
        if ( keep[ i ] ) {
            keep[ i ] = i;
            continue;
        }

        while( --last_keep > i && ! keep[ last_keep ] )
            keep[ last_keep ] = -1;

        vec.set_item( i, std::move( vec[ last_keep ] ) );
        keep[ last_keep ] = i;
        keep[ i ] = -1;
    }

    vec.resize( last_keep );
}

DTP UTP::Point UTP::compute_pos( const Point &p0, const Point &p1, Scalar s0, Scalar s1 ) const {
    return p0 - s0 / ( s1 - s0 ) * ( p1 - p0 );
}

DTP auto UTP::compute_pos( Vec<PI,nb_dims> num_cuts, const auto &get_w ) const {
    using NScalar = DECAYED_TYPE_OF( get_w( w0, CutType::Dirac, i0 ) );
    using NPoint = Vec<NScalar,nb_dims>;

    if constexpr ( nb_dims == 0 ) {
        return NPoint{};
    } else {
        using TM = Vec<Vec<NScalar,nb_dims>,nb_dims>;
        using TV = Vec<NScalar,nb_dims>;

        const auto nw0 = get_w( w0, CutType::Dirac, i0 );

        TM m;
        TV v;
        for( PI i = 0; i < nb_dims; ++i ) {
            const auto &cut = cuts[ num_cuts[ i ] ];
            for( PI j = 0; j < nb_dims; ++j )
                m[ i ][ j ] = cut.dir[ j ];

            if ( cut.type == CutType::Dirac ) {
                auto dir = cut.p1 - p0;
                auto n = norm_2_p2( dir );
                auto s0 = sp( dir, p0 );
                auto s1 = sp( dir, cut.p1 );

                auto nw1 = get_w( cut.w1, cut.type, cut.i1 );

                v[ i ] = s0 + ( 1 + ( nw0 - nw1 ) / n ) / 2 * ( s1 - s0 );
            } else {
                v[ i ] = cut.sp;
            }
        }

        return lu_solve( m, v );
    }
}

DTP UTP::Point UTP::compute_pos( Vec<PI,nb_dims> num_cuts ) const {
    if constexpr ( nb_dims == 0 ) {
        return {};
    } else {
        using TM = Vec<Vec<Scalar,nb_dims>,nb_dims>;
        using TV = Vec<Scalar,nb_dims>;

        TM m;
        TV v;
        for( PI i = 0; i < nb_dims; ++i ) {
            for( PI j = 0; j < nb_dims; ++j )
                m[ i ][ j ] = cuts[ num_cuts[ i ] ].dir[ j ];
            v[ i ] = cuts[ num_cuts[ i ] ].off;
        }

        return lu_solve( m, v );
    }
}

DTP bool UTP::_all_inside( const Point &dir, Scalar off ) {
    constexpr PI simd_size = VertexCoords::simd_size;
    using SimdVec = VertexCoords::SimdVec;

    const PI floor_of_nb_vertices = nb_vertices() / simd_size * simd_size;
    for( PI num_vertex = 0; ; num_vertex += simd_size ) {
        // end of the loop with individual items
        if ( num_vertex == floor_of_nb_vertices ) {
            for( ; ; ++num_vertex ) {
                if ( num_vertex == nb_vertices() )
                    return true;
                if ( sp( vertex_coords[ num_vertex ], dir ) > off )
                    return false;
            }
        }

        // test with simd values
        Scalar *ptr = vertex_coords.data() + vertex_coords.offset( num_vertex );
        SimdVec csp = SimdVec::load_aligned( ptr ) * dir[ 0 ];
        for( int d = 1; d < nb_dims; ++d )
            csp += SimdVec::load_aligned( ptr + d * simd_size ) * dir[ d ];
        if ( xsimd::any( csp > off ) )
            return false;
    }
}

DTP void UTP::_get_sps_and_used_cuts( const Point &dir, Scalar off, const PI op_id ) {
    sps.resize( vertex_coords.size() );
    for( PI num_vertex = 0; num_vertex < nb_vertices(); ++num_vertex ) {
        Scalar s = sp( vertex_coords[ num_vertex ], dir ) - off;
        sps[ num_vertex ] = s;
        bool ext = s > 0;

        if ( ! ext )
            for( const PI &num_cut : vertex_cuts[ num_vertex ] )
                cuts[ num_cut ].op_id = op_id;
    }
}

DTP void UTP::_move_inactive_cuts( const PI op_id ) {
    // sweep the active cuts, and check if it can stay in it this list
    for( int *curr = &last_active_cut; ; curr = &cuts[ *curr ].prev ) {
        // while we're on an inactive cut
        while ( true ) {
            if ( *curr < 0 )
                return;
            if ( cuts[ *curr ].op_id == op_id )
                break;
            // place curr in the inactive set
            last_inactive_cut = std::exchange( *curr, std::exchange( cuts[ *curr ].prev, last_inactive_cut ) );
        }
    }
}

DTP int UTP::_new_unused_cut() {
    if ( last_inactive_cut >= 0 )
        return std::exchange( last_inactive_cut, cuts[ last_inactive_cut ].prev );
    return cuts.push_back_ind();
}

DTP void UTP::_cut( CutType type, const Point &dir, Scalar off, const Point &p1, Scalar w1, PI i1 ) {
    // test if all points are inside, make the scalar products and get used cuts
    if ( _all_inside( dir, off ) )
        return;

    // else, compute and store the scalar product for each vertex. Mark the used cuts
    const PI op_id = ++curr_op_id;
    _get_sps_and_used_cuts( dir, off, op_id );

    for( int curr = last_active_cut; curr >= 0; curr = cuts[ curr ].prev )
        P( "active", curr );
    for( int curr = last_inactive_cut; curr >= 0; curr = cuts[ curr ].prev )
        P( "inactive", curr );

    P( sps );

    // upate active and inactive cuts
    _move_inactive_cuts( op_id );

    for( int curr = last_active_cut; curr >= 0; curr = cuts[ curr ].prev )
        P( "active", curr );
    for( int curr = last_inactive_cut; curr >= 0; curr = cuts[ curr ].prev )
        P( "inactive", curr );

    // choose where to place the new cut
    PI new_cut = _new_unused_cut();

    TODO;
    //

    // used_cuts.resize( cuts.size() );
    // used_cuts.fill( false );
    // for( PI num_vertex = 0; num_vertex < floor_of_nb_vertices; num_vertex += simd_size ) {
    //     Scalar *ptr = vertex_coords.data() + vertex_coords.offset( num_vertex );
    //     SimdVec s = SimdVec::load_aligned( ptr ) * dir[ 0 ];
    //     for( int d = 1; d < nb_dims; ++d )
    //         s += SimdVec::load_aligned( ptr + d * simd_size ) * dir[ d ];
    //     s -= off;

    //     s.store_aligned( sps.data() + num_vertex );

    //     for( PI n = 0; n < simd_size; ++n )
    //         if ( ! ( s.get( n ) > 0 ) )
    //             for( const PI &num_cut : vertex_cuts[ num_vertex + n ] )
    //                 used_cuts[ num_cut ] = true;
    // }
    // for( PI num_vertex = floor_of_nb_vertices; num_vertex < nb_vertices(); ++num_vertex ) {
    //     Scalar s = sp( vertex_coords[ num_vertex ], dir ) - off;

    //     sps[ num_vertex ] = s;

    //     if ( ! ( s > 0 ) )
    //         for( const PI &num_cut : vertex_cuts[ num_vertex ] )
    //             used_cuts[ num_cut ] = true;
    // }


    // // add the new cut
    // PI new_cut;
    // if ( free_cuts.empty() ) {
    //     new_cut = cuts.push_back_ind( type, dir, off, p1, w1, i1 );
    //     edge_map.set_max_PI_value( new_cut );

    //     used_cuts << true;
    // } else {
    //     new_cut = free_cuts.pop_back_val();

    //     cuts.set_item( new_cut, Cut<Scalar,nb_dims>( type, dir, off, p1, w1, i1 ) );
    //     used_cuts[ new_cut ] = true;

    //     edge_map.set_max_PI_value( cuts.size() );
    // }

    // // prepare an edge map to get the first vertex the second time one sees a given edge
    // PI op_id = std::exchange( curr_op_id, curr_op_id + nb_vertices() );

    // // add the new vertices
    // const PI old_nb_vertices = nb_vertices();
    // for( PI n0 = 0; n0 < old_nb_vertices; ++n0 ) {
    //     const bool ext_0 = sps[ n0 ] > 0;
    //     for( PI ind_cut = 0; ind_cut < nb_dims; ++ind_cut ) {
    //         auto edge_cuts = vertex_cuts[ n0 ].without_index( ind_cut );
    //         PI &edge_op_id = edge_map[ edge_cuts ];
    //         if ( edge_op_id >= op_id ) {
    //             const PI n1 = edge_op_id - op_id;
    //             const bool ext_1 = sps[ n1 ] > 0;

    //             if ( ext_0 != ext_1 ) {
    //                 vertex_coords << compute_pos( vertex_coords[ n0 ], vertex_coords[ n1 ], sps[ n0 ], sps[ n1 ] );
    //                 vertex_cuts << edge_cuts.with_pushed_value( new_cut );
    //                 sps.push_back( 1 );
    //             }
    //         } else
    //             edge_op_id = op_id + n0;
    //     }
    // }

    // // remove ext vertices
    // auto remove_ext_vertices = [&]() {
    //     int nb_vertices_to_keep = nb_vertices();
    //     for( int i = 0; i < old_nb_vertices; ++i ) {
    //         if ( sps[ i ] > 0 ) {
    //             while ( true ) {
    //                 if ( --nb_vertices_to_keep <= i ) {
    //                     vertex_coords.resize( nb_vertices_to_keep );
    //                     vertex_cuts.resize( nb_vertices_to_keep );
    //                     return;
    //                 }
    //                 if ( ! ( sps[ nb_vertices_to_keep ] > 0 ) )
    //                     break;
    //             }
    //             vertex_coords.set_item( i, vertex_coords[ nb_vertices_to_keep ] );
    //             vertex_cuts.set_item( i, vertex_cuts[ nb_vertices_to_keep ] );
    //         }
    //     }
    //     vertex_coords.resize( nb_vertices_to_keep );
    //     vertex_cuts.resize( nb_vertices_to_keep );
    // };
    // remove_ext_vertices();

    // // push the new cut
    // auto &cut = cuts[ new_cut ];
    // cut.prev = last_active_cut;
    // cut.type = type;
    // cut.dir = dir;
    // cut.sp = off;
    // cut.p1 = p1;
    // cut.w1 = w1;
    // cut.i1 = i1;

    // last_active_cut = new_cut;
}

DTP void UTP::cut_boundary( const Point &dir, Scalar off, PI num_boundary ) {
    _cut( CutType::Boundary, dir, off, Point{}, Scalar{}, num_boundary );
}

DTP void UTP::cut_dirac( const Point &p1, Scalar w1, PI i1 ) {
    const Point dir = p1 - p0;
    auto n = norm_2_p2( dir );
    auto s0 = sp( dir, p0 );
    auto s1 = sp( dir, p1 );

    auto off = s0 + ( 1 + ( w0 - w1 ) / n ) / 2 * ( s1 - s0 );

    _cut( CutType::Dirac, dir, off, p1, w1, i1 );
}

DTP void UTP::for_each_vertex( const std::function<void( const Point &pos, const Vec<int,nb_dims> &num_cuts )> &f ) const {
    for( PI i = 0; i < nb_vertices(); ++i )
        f( vertex_coords[ i ], vertex_cuts[ i ] );
}

DTP void UTP::for_each_edge( const std::function<void( const Vec<PI,nb_dims-1> &num_cuts, Span<PI,2> vertices )> &f ) const {
    edge_map.set_max_PI_value( cuts.size() );
    PI op_id = std::exchange( curr_op_id, curr_op_id + nb_vertices() );
    for( PI n0 = 0, nv = nb_vertices(); n0 < nv; ++n0 ) {
        for( PI ind_cut = 0; ind_cut < nb_dims; ++ind_cut ) {
            auto edge_cuts = vertex_cuts[ n0 ].without_index( ind_cut );
            PI &edge_op_id = edge_map[ edge_cuts ];
            if ( edge_op_id >= op_id ) {
                const PI n1 = edge_op_id - op_id;
                const PI ns[] = { n1, n0 };
                f( edge_cuts, ns );
            } else
                edge_op_id = op_id + n0;
        }
    }
}

DTP void UTP::for_each_face( const std::function<void( const Vec<PI,nb_dims-2> &num_cuts, Span<PI> vertices )> &f ) const {
    // face => edge list. TODO: optimize
    std::map<Vec<PI,nb_dims-2>,Vec<Vec<PI,2>>,Less> map;
    for_each_edge( [&]( Vec<PI,nb_dims-1> bc, Span<PI,2> vertices ) {
        for( PI nf = 0; nf < nb_dims - 1; ++nf ) {
            Vec<PI,nb_dims-2> bd = bc.without_index( nf );
            map[ bd ] << Vec<PI,2>( vertices );
        }
    } );
    if ( map.empty() )
        return;

    // for each face
    Vec<PI> vertices;
    for( const auto &face : map ) {
        // helper to find an unused vertex
        Vec<bool> unused( FromSizeAndItemValue(), nb_vertices(), true );
        auto find_unused_vertex = [&]() -> Opt<PI> {
            for( const auto &a : face.second )
                if ( unused[ a[ 0 ] ] )
                    return a[ 0 ];
            return {};
        };

        // make edge sweep. TODO: avoid this O(n^2) behavior
        while ( Opt<PI> p = find_unused_vertex() ) {
            unused[ *p ] = false;

            vertices.clear();
            vertices << *p;

            // helper to find the next vertex in the face
            auto find_next_vertex = [&]() -> Opt<PI> {
                for( const auto &a : face.second ) {
                    if ( a[ 0 ] == *p && unused[ a[ 1 ] ] )
                        return a[ 1 ];
                    if ( a[ 1 ] == *p && unused[ a[ 0 ] ] )
                        return a[ 0 ];
                }
                return {};
            };

            //
            while ( Opt<PI> n = find_next_vertex() ) {
                unused[ *n ] = false;
                vertices << *n;
                p = n;
            }

            //
            f( face.first, vertices );
        }
    }
}

// DTP bool UTP::vertex_has_cut( const Vertex<Scalar,nb_dims> &vertex, const std::function<bool( SI point_index )> &outside_cut ) const {
//     for( auto num_cut : vertex.num_cuts )
//         if ( outside_cut( cuts[ num_cut ].i1 ) )
//             return true;
//     return false;
// }

DTP void UTP::add_measure_rec( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex, const auto &positions, PI op_id ) const {
    TODO;
    // if constexpr ( constexpr PI c = num_cuts.ct_size ) {
    //     for( int n = 0; n < num_cuts.size(); ++n ) {
    //         // next item ref
    //         auto next_num_cuts = num_cuts.without_index( n );

    //         // vertex choice for this item
    //         int &next_vertex = item_to_vertex[ next_num_cuts ];
    //         if ( next_vertex < 0 ) {
    //             next_vertex = last_vertex;
    //             continue;
    //         }

    //         // matrix column
    //         const auto &last_pos = positions[ last_vertex ];
    //         const auto &next_pos = positions[ next_vertex ];
    //         for( int d = 0; d < nb_dims; ++d )
    //             M[ d ][ c - 1 ] = next_pos[ d ] - last_pos[ d ];

    //         // recursion
    //         add_measure_rec( res, M, item_to_vertex, next_num_cuts, next_vertex );
    //     }
    // } else {
    //     using std::abs;
    //     res += abs( M.determinant() );
    // }
}

DTP void UTP::add_measure_rec( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex, PI op_id ) const {
    if constexpr ( constexpr PI c = num_cuts.ct_size ) {
        for( int n = 0; n < num_cuts.size(); ++n ) {
            // next item ref
            auto next_num_cuts = num_cuts.without_index( n );

            // vertex choice for this item
            PI &iv = item_to_vertex[ next_num_cuts ];
            if ( iv < op_id ) {
                iv = op_id + last_vertex;
                continue;
            }

            //
            const PI next_vertex = iv - op_id;

            // matrix column
            for( int d = 0; d < nb_dims; ++d )
                M[ d ][ c - 1 ] = vertex_coords( next_vertex, d ) - vertex_coords( last_vertex, d );

            // recursion
            add_measure_rec( res, M, item_to_vertex, next_num_cuts, next_vertex, op_id );
        }
    } else {
        using std::abs;
        res += abs( determinant( M ) );
    }
}

DTP auto UTP::measure( const auto &get_w ) const {
    PI op_id = std::exchange( curr_op_id, curr_op_id + nb_vertices() );
    MapOfUniquePISortedArray<0,nb_dims-1> item_to_vertex;
    item_to_vertex.set_max_PI_value( cuts.size() );

    using NScalar = DECAYED_TYPE_OF( get_w( w0, CutType::Dirac, i0 ) );
    using NPoint = Vec<NScalar,nb_dims>;
    Vec<NPoint> positions( FromReservationSize(), nb_vertices() );
    for( PI i = 0; i < nb_vertices(); ++i )
        positions << compute_pos( vertex_cuts[ i ], get_w );

    NScalar res = 0;
    Vec<Vec<NScalar,nb_dims>,nb_dims> M;
    for( PI i = 0; i < nb_vertices(); ++i ) {
        auto num_cuts = vertex_cuts[ i ];
        std::sort( num_cuts.begin(), num_cuts.end() );
        add_measure_rec( res, M, item_to_vertex, num_cuts, i, positions, op_id );
    }

    Scalar coe = 1;
    for( int d = 2; d <= nb_dims; ++d )
        coe *= d;

    return res / coe;
}

DTP Scalar UTP::measure() const {
    PI op_id = std::exchange( curr_op_id, curr_op_id + cuts.size() );
    MapOfUniquePISortedArray<0,nb_dims-0> item_to_vertex;
    item_to_vertex.set_max_PI_value( cuts.size() );

    Scalar res = 0;
    Vec<Vec<Scalar,nb_dims>,nb_dims> M;
    for( PI i = 0; i < nb_vertices(); ++i ) {
        auto num_cuts = vertex_cuts[ i ];
        std::sort( num_cuts.begin(), num_cuts.end() );
        add_measure_rec( res, M, item_to_vertex, num_cuts, i, op_id );
    }

    Scalar coe = 1;
    for( int d = 2; d <= nb_dims; ++d )
        coe *= d;

    return res / coe;
}

DTP void UTP::display_vtk( VtkOutput &vo, const std::function<void( Vec<Scalar,3> &pt )> &coord_change ) const { //
    auto to_vtk = [&]( const auto &pos ) {
        Vec<Scalar,3> opi;
        for( PI i = 0; i < min( PI( pos.size() ), PI( opi.size() ) ); ++i )
            opi[ i ] = pos[ i ];
        for( PI i = PI( pos.size() ); i < PI( opi.size() ); ++i )
            opi[ i ] = 0;
        coord_change( opi );

        VtkOutput::Pt res;
        for( PI i = 0; i < PI( opi.size() ); ++i )
            res[ i ] = conv( CtType<VtkOutput::TF>(), opi[ i ] );
        return res;
    };

    auto add_item = [&]( int vtk_id, auto vertices ) {
        typename VtkOutput::VTF convex_function;
        typename VtkOutput::VTF is_outside;
        Vec<VtkOutput::Pt> points;
        for( PI num_vertex : vertices ) {
            convex_function << conv( CtType<VtkOutput::TF>(), sp( vertex_coords[ num_vertex ], p0 ) - ( norm_2_p2( p0 ) - w0 ) / 2 );
            is_outside << 0; // vertex_has_cut( *vertex, []( SI v ) { return v < 0; } );
            points << to_vtk( vertex_coords[ num_vertex ] );
        }
        vo.add_polygon( points, { { "convex_function", convex_function }, { "is_outside", is_outside } } ); //
        //if ( p0 && w0 )
        //else
        //    vo.add_polygon( points, { { "is_outside", is_outside } } );
    };

    // edges
    if constexpr ( nb_dims >= 1 ) {
        for_each_edge( [&]( Vec<PI,nb_dims-1> num_cuts, Span<PI,2> vertices ) {
            add_item( VtkOutput::VtkLine, Span<PI>( vertices ) );
        } );
    }

    // faces
    if constexpr ( nb_dims >= 2 ) {
        for_each_face( [&]( Vec<PI,nb_dims-2> bc, Span<PI> vertices ) {
            add_item( VtkOutput::VtkPolygon, vertices );
        } );
    }
}

DTP void UTP::display( Displayer &ds ) const {
    ds.start_object();
    ds.set_next_name( "vertex_coords" ); ds << vertex_coords;
    ds.set_next_name( "vertex_cuts" ); ds << vertex_cuts;
    ds.set_next_name( "cuts" ); ds << cuts;
    ds.end_object();
}

DTP void UTP::display_vtk( VtkOutput &vo ) const {
    return display_vtk( vo, []( Vec<Scalar,3> & ) {} );
}

DTP void UTP::add_cut_types( CountOfCutTypes &cct, const auto &num_cuts, SI nb_bnds ) const {
    for( const PI num_cut : num_cuts ) {
        const SI n_index = cuts[ num_cut ].n_index;

        bool is_int = n_index >= nb_bnds;
        bool is_inf = n_index < 0;
        bool is_bnd = ! ( is_int || is_inf );

        cct.nb_ints += is_int;
        cct.nb_infs += is_inf;
        cct.nb_bnds += is_bnd;
    }
}

DTP void UTP::get_used_fbs( Vec<bool> &used_fs, Vec<bool> &used_bs, PI nb_bnds ) const {
    if ( ! empty() )
        used_fs[ i0 ] = true;
    for_each_vertex( [&]( const Point &pos, Vec<int,nb_dims> num_cuts ) {
        for( PI num_cut : num_cuts ) {
            SI ind = cuts[ num_cut ].n_index;
            if ( ind >= SI( nb_bnds ) )
                used_fs[ ind - nb_bnds ] = true;
            else if ( ind >= 0 )
                used_bs[ ind ] = true;
        }
    } );
}

DTP bool UTP::is_inf() const {
    if ( nb_vertices() == 0 )
        return true;
    for( const auto &num_cuts : vertex_cuts )
        for( auto num_cut : num_cuts )
            if ( cuts[ num_cut ].is_inf() )
                return true;
    return false;
}

DTP bool UTP::contains( const Point &x ) const {
    for( const auto &cut : cuts )
        if ( sp( cut.dir, x ) > cut.sp )
            return false;
    return true;
}

DTP Scalar UTP::height( const Point &point ) const {
    return sp( point, p0 ) - ( norm_2_p2( p0 ) - w0 ) / 2;
}

DTP bool UTP::empty() const {
    return nb_vertices() == 0;
}

#undef DTP
#undef UTP
