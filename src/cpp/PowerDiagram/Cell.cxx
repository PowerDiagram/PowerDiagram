#pragma once

#include <tl/support/containers/operators/determinant.h>
#include <tl/support/containers/operators/lu_solve.h>
#include <tl/support/containers/operators/norm_2.h>
#include <tl/support/containers/operators/sp.h>
#include <tl/support/containers/SmallVec.h>
#include <tl/support/containers/CtRange.h>
#include <tl/support/containers/Opt.h>
#include <tl/support/compare.h>
#include <tl/support/conv.h>
#include "Cell.h"

#include <tl/support/P.h>
 
// #include <eigen3/Eigen/LU>

#define DTP template<class TS,int nb_dims>
#define UTP Cell<TS,nb_dims>

DTP UTP::Cell() {
    vertex_indices.reserve( 128 );
    vertices.reserve( 128 );
    cuts.reserve( 128 );

    nb_active_vertices = 0;

    sps.reserve( 128 );

    num_cut_map.for_each_item( []( auto &obj ) { obj.map.prepare_for( 32 ); } );
    num_cut_oid = 0;
}

DTP void UTP::init_geometry_from( const Cell &that ) {
    // limits
    min_pos = that.min_pos;
    max_pos = that.max_pos;

    // vertices
    nb_active_vertices = that.nb_active_vertices;
    vertex_indices.clear();
    vertices.clear();

    for( PI i = 0; i < that.nb_active_vertices; ++i ) {
        vertices << that.vertices[ that.vertex_indices[ i ] ];
        vertex_indices << i;
    }

    // cuts
    cuts = that.cuts;
}

DTP void UTP::init_geometry_to_encompass( const Point &mi, const Point &ma ) {
    // first value. max_pos will be updated
    min_pos = mi - ( ma - mi ) / 2;
    max_pos = ma + ( ma - mi ) / 2;

    // cuts
    cuts.clear();
    for( int d = 0; d < nb_dims; ++d ) {
        Point dir( FromInitFunctionOnIndex(), [&]( TS *res, PI i ) { *res = ( i == d ? -1 : 0 ); } );
        cuts.push_back( CutType::Infinity, dir, sp( min_pos, dir ), Point{}, TS{ 0 }, PI( d ) );
    }

    Point dir( FromItemValue(), 1 );
    cuts.push_back( CutType::Infinity, dir, sp( max_pos, dir ), Point{}, TS{ 0 }, PI( nb_dims ) );

    // vertices
    nb_active_vertices = nb_dims + 1;
    vertex_indices.clear();
    vertices.clear();
    for( int nc_0 = 0; nc_0 < nb_dims + 1; ++nc_0 ) {
        Vec<PI32,nb_dims> num_cuts( FromItemValue(), 0 );
        for( PI32 i = 0; i < nc_0; ++i )
            num_cuts[ i ] = i;
        for( PI32 i = nc_0 + 1; i < nb_dims + 1; ++i )
            num_cuts[ i - 1 ] = i;

        Point pos = compute_pos( num_cuts );
        max_pos = max( max_pos, pos );

        vertices.push_back( num_cuts, pos );
        vertex_indices << nc_0;
    }
}

// DTP void UTP::apply_corr( auto &v0, auto &v1, Vec<int> &keep ) {
//     int last_keep = v0.size();
//     for( int i = 0; i < last_keep; ++i ) {
//         if ( keep[ i ] ) {
//             keep[ i ] = i;
//             continue;
//         }

//         while( --last_keep > i && ! keep[ last_keep ] )
//             keep[ last_keep ] = -1;

//         v0.set_item( i, std::move( v0[ last_keep ] ) );
//         v1.set_item( i, std::move( v1[ last_keep ] ) );
//         keep[ last_keep ] = i;
//         keep[ i ] = -1;
//     }

//     v0.resize( last_keep );
//     v1.resize( last_keep );
// }

// DTP void UTP::apply_corr( Vec<int> &keep, auto &vec ) {
//     int last_keep = vec.size();
//     for( int i = 0; i < last_keep; ++i ) {
//         if ( keep[ i ] ) {
//             keep[ i ] = i;
//             continue;
//         }

//         while( --last_keep > i && ! keep[ last_keep ] )
//             keep[ last_keep ] = -1;

//         vec.set_item( i, std::move( vec[ last_keep ] ) );
//         keep[ last_keep ] = i;
//         keep[ i ] = -1;
//     }

//     vec.resize( last_keep );
// }

DTP UTP::Point UTP::compute_pos( const Point &p0, const Point &p1, TS s0, TS s1 ) const {
    return p0 - s0 / ( s1 - s0 ) * ( p1 - p0 );
}

DTP UTP::Point UTP::compute_pos( Vec<PI,nb_dims> num_cuts ) const {
    if constexpr ( nb_dims == 0 ) {
        return {};
    } else {
        using TM = Vec<Vec<TS,nb_dims>,nb_dims>;
        using TV = Vec<TS,nb_dims>;

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

DTP bool UTP::_all_inside( const Point &dir, TS off ) {
    // constexpr PI simd_size = VertexCoords::simd_size;
    // using SimdVec = VertexCoords::SimdVec;

    // const PI floor_of_nb_vertices = nb_vertices() / simd_size * simd_size;
    // for( PI num_vertex = 0; ; num_vertex += simd_size ) {
    //     // end of the loop with individual items
    //     if ( num_vertex == floor_of_nb_vertices ) {
    //         for( ; ; ++num_vertex ) {
    //             if ( num_vertex == nb_vertices() )
    //                 return true;
    //             if ( sp( vertex_coords[ num_vertex ], dir ) > off )
    //                 return false;
    //         }
    //     }

    //     // test with simd values
    //     TS *ptr = vertex_coords.data() + vertex_coords.offset( num_vertex );
    //     SimdVec csp = SimdVec::load_aligned( ptr ) * dir[ 0 ];
    //     for( int d = 1; d < nb_dims; ++d )
    //         csp += SimdVec::load_aligned( ptr + d * simd_size ) * dir[ d ];
    //     if ( xsimd::any( csp > off ) )
    //         return false;
    // }
    return false;
}

// DTP void UTP::_get_sps( const Point &dir, TS off ) {
//     constexpr PI simd_size = VertexCoords::simd_size;
//     using SimdVec = VertexCoords::SimdVec;

//     sps.resize( nb_vertices() );

//     const PI floor_of_nb_vertices = nb_vertices() / simd_size * simd_size;
//     for( PI num_vertex = 0; num_vertex < floor_of_nb_vertices; num_vertex += simd_size ) {
//         TS *ptr = vertex_coords.data() + vertex_coords.offset( num_vertex );
//         SimdVec s = SimdVec::load_aligned( ptr ) * dir[ 0 ];
//         for( int d = 1; d < nb_dims; ++d )
//             s += SimdVec::load_aligned( ptr + d * simd_size ) * dir[ d ];
//         s -= off;

//         s.store_aligned( sps.data() + num_vertex );
//     }

//     for( PI num_vertex = floor_of_nb_vertices; num_vertex < nb_vertices(); ++num_vertex )
//         sps[ num_vertex ] = sp( vertex_coords[ num_vertex ], dir ) - off;
// }

DTP void UTP::_add_cut_vertices( PI32 new_cut ) {
    TODO;
    // // prepare an edge map to get the first vertex the second time one sees a given edge
    // PI op_id = std::exchange( ++curr_op_id, curr_op_id + nb_vertices() );
    // edge_map.set_max_PI_value( new_cut );

    // // add the new vertices
    // const PI old_nb_vertices = nb_vertices();
    // for( PI n0 = 0; n0 < old_nb_vertices; ++n0 ) {
    //     const bool ext_0 = sps[ n0 ] > 0;
    //     CtRange<0,nb_dims>::for_each_item( [&]( auto ind_cut ) {
    //         PI &edge_op_id = edge_map.at_without_index( vertex_cuts[ n0 ], ind_cut );
    //         if ( edge_op_id >= op_id ) {
    //             const PI n1 = edge_op_id - op_id;
    //             const bool ext_1 = sps[ n1 ] > 0;

    //             if ( ext_0 != ext_1 ) {
    //                 vertex_coords << compute_pos( vertex_coords[ n0 ], vertex_coords[ n1 ], sps[ n0 ], sps[ n1 ] );

    //                 // auto edge_cuts = vertex_cuts[ n0 ].without_index( ind_cut );
    //                 auto &cut = *vertex_cuts.push_back(); // << edge_cuts.with_pushed_value( new_cut );
    //                 CtRange<0,nb_dims-1>::for_each_item( [&]( auto j ) {
    //                     cut[ j ] = vertex_cuts[ n0 ][ PI( j ) + ( PI( j ) >= ind_cut ) ];
    //                 } );
    //                 cut[ nb_dims-1 ] = new_cut;
    //             }
    //         } else
    //             edge_op_id = op_id + n0;
    //     } );
    // }
}

DTP void UTP::_cut( CutType type, const Point &dir, TS off, const Point &p1, TS w1, PI i1 ) {
    TODO;
    // // test if all points are inside, make the TS products and get used cuts
    // if ( _all_inside( dir, off ) )
    //     return;

    // // else, compute and store the TS product for each vertex. Mark the used cuts
    // _get_sps( dir, off );

    // // add the new cut
    // PI new_cut = cuts.push_back_ind( type, dir, off, p1, w1, i1 );

    // // add the new vertices
    // const PI old_nb_vertices = nb_vertices();
    // _add_cut_vertices( new_cut );

    // // remove ext ones
    // // PI new_nb_vertices = _remove_ext_vertices( old_nb_vertices );
    // // vertex_coords.resize( new_nb_vertices );
    // // vertex_cuts.resize( new_nb_vertices );

    // keep.clear();
    // for( TS v : sps )
    //     keep << ( v <= 0 );
    // for( PI i = old_nb_vertices; i < nb_vertices(); ++i )
    //     keep << 1;
    // apply_corr( vertex_coords, vertex_cuts, keep );
}

DTP void UTP::cut_boundary( const Point &dir, TS off, PI num_boundary ) {
    _cut( CutType::Boundary, dir, off, Point{}, TS{}, num_boundary );
}

DTP void UTP::cut_dirac( const Point &p1, TS w1, PI i1 ) {
    const Point dir = p1 - p0;
    auto n = norm_2_p2( dir );
    auto s0 = sp( dir, p0 );
    auto s1 = sp( dir, p1 );

    auto off = s0 + ( 1 + ( w0 - w1 ) / n ) / 2 * ( s1 - s0 );

    _cut( CutType::Dirac, dir, off, p1, w1, i1 );
}

DTP void UTP::for_each_vertex( const std::function<void( const Vertex &vertex )> &f ) const {
    //for( PI32 ind : vertex_indices )
    for( PI32 i = 0; i < nb_active_vertices; ++i )
        f( vertices[ vertex_indices[ i ] ] );
}

DTP void UTP::for_each_edge( const std::function<void( const Vec<PI32,nb_dims-1> &num_cuts, Span<const Vertex *,2> vertices )> &f ) const {
    PI op_id = std::exchange( ++num_cut_oid, num_cut_oid + nb_vertices() );
    auto &edge_map = num_cut_map[ CtInt<nb_dims-1>() ].map;
    edge_map.prepare_for( cuts.size() );
    for( PI32 ni = 0; ni < nb_active_vertices; ++ni ) {
        PI32 n0 = vertex_indices[ ni ];
        const Vertex &vertex = vertices[ n0 ];

        CtRange<0,nb_dims>::for_each_item( [&]( auto ind_cut ) {
            auto edge_cuts = vertex.num_cuts.without_index( ind_cut );
            PI32 &edge_op_id = edge_map[ edge_cuts ];
            if ( edge_op_id >= op_id ) {
                const PI n1 = edge_op_id - op_id;
                const Vertex *ns[] = {
                    &vertices[ n1 ],
                    &vertex
                };
                f( edge_cuts, ns );
            } else
                edge_op_id = op_id + n0;
        } );
    }
}

DTP void UTP::for_each_face( const std::function<void( const Vec<PI32,nb_dims-2> &num_cuts, Span<const Vertex *> vertices )> &f ) const {
    // sibling for each vertex, for each face
    struct FaceInfo { Vec<SmallVec<PI,2>> siblings; PI start; };
    std::map<Vec<PI32,nb_dims-2>,FaceInfo,Less> face_map;
    for_each_edge( [&]( const Vec<PI32,nb_dims-1> &edge_cuts, auto vs ) {
        // for each connected face
        for( PI i = 0; i < nb_dims - 1; ++i ) {
            Vec<PI32,nb_dims-2> face_cuts = edge_cuts.without_index( i );
            auto &fi = face_map[ face_cuts ];

            // store connected vertices
            fi.siblings.resize( vertices.size() );
            PI v0 = vs[ 0 ] - vertices.data();
            PI v1 = vs[ 1 ] - vertices.data();
            fi.siblings[ v0 ] << v1;
            fi.siblings[ v1 ] << v0;
            fi.start = v0;
        }
    } );

    //
    Vec<const Vertex *> vs;
    for( auto &p: face_map ) {
        const Vec<PI32,nb_dims-2> &face_cuts = p.first;
        const FaceInfo &fi = p.second;

        vs.clear();
        for( PI n = fi.start, j = 0; j < 10; ++j ) {
            vs << &vertices[ n ];

            const PI s = vs.size() > 1 && vs[ vs.size() - 2 ] == &vertices[ fi.siblings[ n ][ 0 ] ];
            n = fi.siblings[ n ][ s ];

            if ( n == fi.start )
                break;
        }

        f( face_cuts, vs );
    }
}

DTP void UTP::add_measure_rec( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex, PI op_id ) const {
    TODO;
    // if constexpr ( constexpr PI c = num_cuts.ct_size ) {
    //     for( int n = 0; n < num_cuts.size(); ++n ) {
    //         // next item ref
    //         auto next_num_cuts = num_cuts.without_index( n );

    //         // vertex choice for this item
    //         PI &iv = item_to_vertex[ next_num_cuts ];
    //         if ( iv < op_id ) {
    //             iv = op_id + last_vertex;
    //             continue;
    //         }

    //         //
    //         const PI next_vertex = iv - op_id;

    //         // matrix column
    //         for( int d = 0; d < nb_dims; ++d )
    //             M[ d ][ c - 1 ] = vertex_coords( next_vertex, d ) - vertex_coords( last_vertex, d );

    //         // recursion
    //         add_measure_rec( res, M, item_to_vertex, next_num_cuts, next_vertex, op_id );
    //     }
    // } else {
    //     using std::abs;
    //     res += abs( determinant( M ) );
    // }
}

DTP TS UTP::measure() const {
    TODO;
    // PI op_id = std::exchange( curr_op_id, curr_op_id + cuts.size() );
    // MapOfUniquePISortedArray<0,nb_dims-0> item_to_vertex;
    // item_to_vertex.set_max_PI_value( cuts.size() );

    // TS res = 0;
    // Vec<Vec<TS,nb_dims>,nb_dims> M;
    // for( PI i = 0; i < nb_vertices(); ++i ) {
    //     auto num_cuts = vertex_cuts[ i ];
    //     std::sort( num_cuts.begin(), num_cuts.end() );
    //     add_measure_rec( res, M, item_to_vertex, num_cuts, i, op_id );
    // }

    // TS coe = 1;
    // for( int d = 2; d <= nb_dims; ++d )
    //     coe *= d;

    // return res / coe;
    return 0;
}

DTP void UTP::display_vtk( VtkOutput &vo, const std::function<Vec<VtkOutput::TF,3>( const Point &pt )> &coord_change ) const { //
    TODO;
    // auto to_vtk = [&]( const auto &pos ) {
    //     Vec<TS,3> opi;
    //     for( PI i = 0; i < min( PI( pos.size() ), PI( opi.size() ) ); ++i )
    //         opi[ i ] = pos[ i ];
    //     for( PI i = PI( pos.size() ); i < PI( opi.size() ); ++i )
    //         opi[ i ] = 0;
    //     coord_change( opi );

    //     VtkOutput::Pt res;
    //     for( PI i = 0; i < PI( opi.size() ); ++i )
    //         res[ i ] = conv( CtType<VtkOutput::TF>(), opi[ i ] );
    //     return res;
    // };

    // auto add_item = [&]( int vtk_id, auto vertices ) {
    //     typename VtkOutput::VTF convex_function;
    //     typename VtkOutput::VTF is_outside;
    //     Vec<VtkOutput::Pt> points;
    //     for( PI num_vertex : vertices ) {
    //         convex_function << conv( CtType<VtkOutput::TF>(), sp( vertex_coords[ num_vertex ], p0 ) - ( norm_2_p2( p0 ) - w0 ) / 2 );
    //         is_outside << 0; // vertex_has_cut( *vertex, []( SI v ) { return v < 0; } );
    //         points << to_vtk( vertex_coords[ num_vertex ] );
    //     }
    //     vo.add_polygon( points, { { "convex_function", convex_function }, { "is_outside", is_outside } } ); //
    //     //if ( p0 && w0 )
    //     //else
    //     //    vo.add_polygon( points, { { "is_outside", is_outside } } );
    // };

    // // edges
    // if constexpr ( nb_dims >= 1 ) {
    //     for_each_edge( [&]( Vec<PI,nb_dims-1> num_cuts, Span<PI,2> vertices ) {
    //         add_item( VtkOutput::VtkLine, Span<PI>( vertices ) );
    //     } );
    // }

    // // faces
    // if constexpr ( nb_dims >= 2 ) {
    //     for_each_face( [&]( Vec<PI,nb_dims-2> bc, Span<PI> vertices ) {
    //         add_item( VtkOutput::VtkPolygon, vertices );
    //     } );
    // }
}

DTP void UTP::display_vtk( VtkOutput &vo ) const {
    return display_vtk( vo, []( const Point &pos ) {
        Vec<VtkOutput::TF,3> res;
        for( PI i = 0; i < min( PI( pos.size() ), PI( res.size() ) ); ++i )
            res[ i ] = pos[ i ];
        for( PI i = PI( pos.size() ); i < PI( res.size() ); ++i )
            res[ i ] = 0;
        return res;
    } );
}

DTP bool UTP::is_inf() const {
    if ( nb_vertices() == 0 )
        return true;
    TODO;
    // for( const auto &num_cuts : vertex_cuts )
    //     for( auto num_cut : num_cuts )
    //         if ( cuts[ num_cut ].is_inf() )
    //             return true;
    return false;
}

DTP bool UTP::contains( const Point &x ) const {
    for( const auto &cut : cuts )
        if ( sp( cut.dir, x ) > cut.off )
            return false;
    return true;
}

DTP TS UTP::height( const Point &point ) const {
    return sp( point, p0 ) - ( norm_2_p2( p0 ) - w0 ) / 2;
}

DTP bool UTP::empty() const {
    return nb_vertices() == 0;
}

#undef DTP
#undef UTP
