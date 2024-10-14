#pragma once

#include <tl/support/operators/determinant.h>
#include <tl/support/operators/lu_solve.h>
#include <tl/support/operators/norm_2.h>
#include <tl/support/operators/any.h>
#include <tl/support/operators/sp.h>

#include <tl/support/containers/SmallVec.h>
#include <tl/support/containers/CtRange.h>
#include <tl/support/containers/Opt.h>

#include <tl/support/ASSERT.h>

#include <tl/support/compare.h>
#include <tl/support/conv.h>

// #include <limits>

#include "Cell.h"

#include <tl/support/P.h>
 
// #include <eigen3/Eigen/LU>

#define DTP template<class Config>
#define UTP Cell<Config>

DTP UTP::Cell() {
    vertex_coords.reserve( 128 );
    vertex_cuts.reserve( 128 );
    cuts.reserve( 128 );

    sps.reserve( 128 );

    num_cut_map.for_each_item( []( auto &obj ) { obj.map.prepare_for( 128 ); } );
    num_cut_oid = 1;
}

DTP void UTP::init_geometry_from( const Cell &that ) {
    // limits
    if ( Config::use_AABB_bounds_on_cells ) {
        min_pos = that.min_pos;
        max_pos = that.max_pos;
    }

    // vertices
    vertex_coords = that.vertex_coords;
    vertex_cuts = that.vertex_cuts;

    // cuts
    cuts = that.cuts;
}

DTP void UTP::init_geometry_to_encompass( const Pt &mi, const Pt &ma ) {
    // first value. max_pos will be updated
    min_pos = mi - ( ma - mi ) / 2;
    max_pos = ma + ( ma - mi ) / 2;

    // cuts
    cuts.clear();
    for( int d = 0; d < nb_dims; ++d ) {
        Pt dir( FromInitFunctionOnIndex(), [&]( TF *res, PI i ) { *res = ( i == d ? -1 : 0 ); } );
        cuts.push_back( CutType::Infinity, dir, sp( min_pos, dir ), Pt{}, TF{ 0 }, PI( d ), nullptr, 0 );
    }

    Pt dir( FromItemValue(), 1 );
    cuts.push_back( CutType::Infinity, dir, sp( max_pos, dir ), Pt{}, TF{ 0 }, PI( nb_dims ), nullptr, 0 );

    // vertices
    vertex_coords.clear();
    vertex_cuts.clear();
    for( int nc_0 = 0; nc_0 < nb_dims + 1; ++nc_0 ) {
        Vec<PI32,nb_dims> num_cuts( FromItemValue(), 0 );
        for( PI32 i = 0; i < nc_0; ++i )
            num_cuts[ i ] = i;
        for( PI32 i = nc_0 + 1; i < nb_dims + 1; ++i )
            num_cuts[ i - 1 ] = i;

        Pt coords = compute_pos( num_cuts );
        max_pos = max( max_pos, coords );

        vertex_coords << coords;
        vertex_cuts << num_cuts;
    }
}

DTP UTP::Pt UTP::compute_pos( const Pt &p0, const Pt &p1, TF s0, TF s1 ) const {
    return p0 - s0 / ( s1 - s0 ) * ( p1 - p0 );
}

DTP UTP::Pt UTP::compute_pos( Vec<PI,nb_dims> num_cuts ) const {
    if constexpr ( nb_dims == 0 ) {
        return {};
    } else {
        using TM = Vec<Vec<TF,nb_dims>,nb_dims>;
        using TV = Vec<TF,nb_dims>;

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

DTP bool UTP::_all_inside( const Pt &dir, TF off ) {
    if ( Config::use_AABB_bounds_on_cells ) {
        TF res = 0;
        for( PI d = 0; d < nb_dims; ++d ) {
            res += ( max_pos[ d ] + min_pos[ d ] ) * dir[ d ] / 2;
            res += ( max_pos[ d ] - min_pos[ d ] ) * abs( dir[ d ] ) / 2;
        }
        if ( res <= off )
            return true;
    }

    for( PI ni = 0; ni < nb_vertices(); ++ni )
        if ( sp( vertex_coords[ ni ], dir ) > off )
            return false;

    // // constexpr PI simd_size = VertexCoords::simd_size;
    // // using SimdVec = VertexCoords::SimdVec;

    // // const PI floor_of_nb_vertices = nb_vertices() / simd_size * simd_size;
    // // for( PI num_vertex = 0; ; num_vertex += simd_size ) {
    // //     // end of the loop with individual items
    // //     if ( num_vertex == floor_of_nb_vertices ) {
    // //         for( ; ; ++num_vertex ) {
    // //             if ( num_vertex == nb_vertices() )
    // //                 return true;
    // //             if ( sp( vertex_coords[ num_vertex ], dir ) > off )
    // //                 return false;
    // //         }
    // //     }

    // //     // test with simd values
    // //     TF *ptr = vertex_coords.data() + vertex_coords.offset( num_vertex );
    // //     SimdVec csp = SimdVec::load_aligned( ptr ) * dir[ 0 ];
    // //     for( int d = 1; d < nb_dims; ++d )
    // //         csp += SimdVec::load_aligned( ptr + d * simd_size ) * dir[ d ];
    // //     if ( xsimd::any( csp > off ) )
    // //         return false;
    // // }

    return true;
}

// DTP void UTP::_get_sps( const Point &dir, TF off ) {
//     constexpr PI simd_size = VertexCoords::simd_size;
//     using SimdVec = VertexCoords::SimdVec;

//     sps.resize( nb_vertices() );

//     const PI floor_of_nb_vertices = nb_vertices() / simd_size * simd_size;
//     for( PI num_vertex = 0; num_vertex < floor_of_nb_vertices; num_vertex += simd_size ) {
//         TF *ptr = vertex_coords.data() + vertex_coords.offset( num_vertex );
//         SimdVec s = SimdVec::load_aligned( ptr ) * dir[ 0 ];
//         for( int d = 1; d < nb_dims; ++d )
//             s += SimdVec::load_aligned( ptr + d * simd_size ) * dir[ d ];
//         s -= off;

//         s.store_aligned( sps.data() + num_vertex );
//     }

//     for( PI num_vertex = floor_of_nb_vertices; num_vertex < nb_vertices(); ++num_vertex )
//         sps[ num_vertex ] = sp( vertex_coords[ num_vertex ], dir ) - off;
// }

DTP PI UTP::new_cut_oid( PI s ) const {
    // reservation for the test
    ++num_cut_oid;

    // + room for some PI data
    return std::exchange( num_cut_oid, num_cut_oid + s );
}

DTP typename UTP::TF UTP::for_each_cut_with_measure( const std::function<void( const CellCut<Config> &cut, TF measure )> &f ) const {
    num_cut_map.for_each_item( [&]( auto &obj ) { obj.map.prepare_for( cuts.size() ); } );
    PI op_id = new_cut_oid( nb_vertices() );

    Vec<TF> measure_for_each_cut( FromSizeAndItemValue(), cuts.size(), 0 );

    TF res = 0;
    Vec<Vec<TF,nb_dims>,nb_dims> M;
    for( PI n = 0; n < nb_vertices(); ++n )
        add_measure_rec( res, M, vertex_cuts[ n ], n, op_id, measure_for_each_cut );

    // cuts
    TF coe = 1;
    for( int d = 2; d + 1 <= nb_dims; ++d )
        coe *= d;

    for( PI num_cut = 0; num_cut < cuts.size(); ++num_cut )
        if ( TF m = measure_for_each_cut[ num_cut ] )
            f( cuts[ num_cut ], m / coe );

    // cell
    coe *= nb_dims;
    return res / coe;
}

DTP void UTP::get_prev_cut_info( PrevCutInfo<Config> &pci ) {
    memory_compaction();

    pci.by_leaf.clear();
    for( const CellCut<Config> &cut : cuts ) {
        if ( cut.type == CutType::Dirac ) {
            if ( pci.by_leaf.empty() || pci.by_leaf.back().first != cut.ptr )
                pci.by_leaf.push_back( cut.ptr, PI32( 0 ) );
            PI32 &used = pci.by_leaf.back().second;
            used |= ( PI32( 1 ) << cut.num_in_ptr );
        }
    }

    std::sort( pci.by_leaf.begin(), pci.by_leaf.end(), []( const auto &a, const auto &b ) {
        return a.first < b.first;
    } );
}

DTP void UTP::_add_cut_vertices( const Pt &dir, TF off, PI32 new_cut ) {
    // prepare an edge map to get the first vertex the second time one sees a given edge
    auto &edge_map = num_cut_map[ CtInt<nb_dims-1>() ].map;
    const PI op_id = new_cut_oid( nb_vertices() );
    edge_map.prepare_for( cuts.size() );

    sps.resize( nb_vertices() );

    // preparation for the new bounds 
    if ( Config::use_AABB_bounds_on_cells ) {
        max_pos = { FromItemValue(), std::numeric_limits<TF>::lowest() };
        min_pos = { FromItemValue(), std::numeric_limits<TF>::max   () };
    }

    // add the new vertices
    const PI old_nb_vertices = nb_vertices();
    for( PI n0 = 0; n0 < old_nb_vertices; ++n0  ) {
        const auto c0 = vertex_cuts[ n0 ].inds;
        const Pt   p0 = vertex_coords[ n0 ];
        const TF   s0 = sp( p0, dir ) - off;
        const bool e0 = s0 > 0;

        sps[ n0 ] = s0;

        // if ext, move the vertex ref to [ new inactive vertices ]
        if ( Config::use_AABB_bounds_on_cells && ! e0 ) {
            max_pos = max( max_pos, p0 );
            min_pos = min( min_pos, p0 );
        }

        //
        CtRange<0,nb_dims>::for_each_item( [&]( auto ind_cut ) {
            // PI &edge_op_id = edge_map.at_without_index( vertices[ n0 ].num_cuts, ind_cut );
            Vec<PI32,nb_dims-1> edge_cuts = c0.without_index( ind_cut );
            auto &edge_op_id = edge_map[ edge_cuts ];
            if ( edge_op_id >= op_id ) {
                const PI32 n1 = edge_op_id - op_id;
                const Pt   p1 = vertex_coords[ n1 ];
                const TF   s1 = sps[ n1 ];
                const bool e1 = s1 > 0;

                if ( e0 != e1 ) {
                    // data for the new vertex
                    auto cn = edge_cuts.with_pushed_value( new_cut );
                    auto pn = compute_pos( p0, p1, s0, s1 );

                    // bounds
                    if ( Config::use_AABB_bounds_on_cells ) {
                        max_pos = max( max_pos, pn );
                        min_pos = min( min_pos, pn );
                    }

                    // append/insert the new
                    vertex_cuts << VertexCut{ cn };
                    vertex_coords << pn;
                }
            } else
                edge_op_id = op_id + n0;
        } );
    }

    // remove ext vertices
    _remove_ext_vertices( old_nb_vertices );
}

DTP void UTP::_cut( CutType type, const Pt &dir, TF off, const Pt &p1, TF w1, PI i1, Ptree *ptr, PI32 num_in_ptr ) {
    // test if all points are inside, make the TF products and get used cuts
    if ( _all_inside( dir, off ) )
        return;

    // store the new cut
    PI new_cut = cuts.push_back_ind( type, dir, off, p1, w1, i1, ptr, num_in_ptr );

    // make the new vertices + deref the ext ones
    _add_cut_vertices( dir, off, new_cut );
}

DTP void UTP::memory_compaction() {
    // update active_cuts
    Vec<PI32> active_cuts( FromSizeAndItemValue(), cuts.size(), false );
    for( PI i = 0; i < nb_vertices(); ++i )
        for( auto num_cut : vertex_cuts[ i ].inds )
            active_cuts[ num_cut ] = true;

    // update cuts + transform active_cuts as cut index correction
    PI nb_cuts = 0;
    for( PI num_cut = 0; num_cut < cuts.size(); ++num_cut ) {
        if ( active_cuts[ num_cut ] ) {
            if ( nb_cuts != num_cut )
                cuts[ nb_cuts ] = std::move( cuts[ num_cut ] );
            active_cuts[ num_cut ] = nb_cuts++;
        }
    }

    cuts.resize( nb_cuts );

    // num cuts
    for( VertexCut &v : vertex_cuts )
        for( PI32 &num_cut : v.num_cuts )
            num_cut = active_cuts[ num_cut ];
}

DTP void UTP::cut_boundary( const Pt &dir, TF off, PI num_boundary ) {
    _cut( CutType::Boundary, dir, off, Pt{}, TF{}, num_boundary, nullptr, 0 );
}

DTP void UTP::cut_dirac( const Pt &p1, TF w1, PI i1, Ptree *ptr, PI32 num_in_ptr ) {
    const Pt dir = p1 - p0;
    auto n = norm_2_p2( dir );
    auto s0 = sp( dir, p0 );
    auto s1 = sp( dir, p1 );

    auto off = s0 + ( 1 + ( w0 - w1 ) / n ) / 2 * ( s1 - s0 );

    _cut( CutType::Dirac, dir, off, p1, w1, i1, ptr, num_in_ptr );
}

// DTP bool UTP::test_each_vertex( const std::function<bool( const Vertex &vertex )> &f ) const {
//     for( PI32 i = 0; i < nb_active_vertices; ++i )
//         if ( f( vertices[ vertex_indices[ i ] ] ) )
//             return true;
//     return false;
// }

// DTP void UTP::for_each_vertex( const std::function<void( const Vertex &vertex )> &f ) const {
//     for( PI32 i = 0; i < nb_active_vertices; ++i )
//         f( vertices[ vertex_indices[ i ] ] );
// }

DTP void UTP::for_each_edge( const std::function<void( const Vec<PI32,nb_dims-1> &num_cuts, Span<PI32,2> num_vertices )> &f ) const {
    auto &edge_map = num_cut_map[ CtInt<nb_dims-1>() ].map;
    const PI op_id = new_cut_oid( nb_vertices() );
    edge_map.prepare_for( cuts.size() );

    for( PI32 n0 = 0; n0 < nb_vertices(); ++n0 ) {
        CtRange<0,nb_dims>::for_each_item( [&]( auto ind_cut ) {
            auto edge_cuts = vertex_cuts[ n0 ].inds.without_index( ind_cut );
            PI &edge_op_id = edge_map[ edge_cuts ];

            if ( edge_op_id >= op_id ) {
                const PI32 ns[] = { edge_op_id - op_id, n0 };
                f( edge_cuts, ns );
            } else
                edge_op_id = op_id + n0;
        } );
    }
}

DTP void UTP::for_each_face( const std::function<void( const Vec<PI32,nb_dims-2> &num_cuts, Span<PI32> vertices )> &f ) const {
    // sibling for each vertex (index in `vertices`), for each face
    struct FaceInfo { Vec<SmallVec<PI,2>> siblings; PI start; };
    std::map<Vec<PI32,nb_dims-2>,FaceInfo,Less> face_map;
    for_each_edge( [&]( const Vec<PI32,nb_dims-1> &edge_cuts, Span<PI32,2> vs ) {
        // for each connected face
        for( PI i = 0; i < nb_dims - 1; ++i ) {
            Vec<PI32,nb_dims-2> face_cuts = edge_cuts.without_index( i );
            auto &fi = face_map[ face_cuts ];

            // store connected vertices
            fi.siblings.resize( nb_vertices() );
            fi.siblings[ vs[ 0 ] ] << vs[ 1 ];
            fi.siblings[ vs[ 1 ] ] << vs[ 0 ];
            fi.start = vs[ 0 ];
        }
    } );

    // for each face
    Vec<PI32> vs;
    for( const auto &p: face_map ) {
        const Vec<PI32,nb_dims-2> &face_cuts = p.first;
        const FaceInfo &fi = p.second;

        // get the loop
        vs.clear();
        for( PI n = fi.start, j = 0; ; ++j ) {
            vs << n;

            const PI s = vs.size() > 1 && vs[ vs.size() - 2 ] == fi.siblings[ n ][ 0 ];
            n = fi.siblings[ n ][ s ];

            if ( n == fi.start )
                break;
        }

        // call f
        f( face_cuts, vs );
    }
}

DTP void UTP::add_measure_rec( auto &res, auto &M, const auto &num_cuts, PI32 prev_vertex, PI op_id, Vec<TF> &measure_for_each_cut ) const {
    using std::sqrt;
    using std::abs;
    using std::pow;

    if constexpr ( constexpr PI c = num_cuts.ct_size ) {
        //
        if ( c == 1 ) {
            Vec<Vec<TF,nb_dims-1>,nb_dims> woc( FromInitFunctionOnIndex(), [&]( Vec<TF,nb_dims-1> *v, PI i ) {
                new ( v ) Vec<TF,nb_dims-1>( M[ i ].without_index( CtInt<0>() ) );
            } );

            TF loc = 0;
            CtRange<0,nb_dims>::for_each_item( [&]( auto r ) {
                auto N = woc.without_index( r );
                loc += pow( determinant( N ), 2 );
            } );
            measure_for_each_cut[ num_cuts[ 0 ] ] += sqrt( loc );
        }

        //
        CtRange<0,c>::for_each_item( [&]( auto n ) {
            // next item ref
            auto next_num_cuts = num_cuts.without_index( n );

            // vertex choice for this item
            auto &iv = num_cut_map[ CtInt<c-1>() ].map[ next_num_cuts ];
            if ( iv < op_id ) {
                iv = op_id + prev_vertex;
                return;
            }

            //
            const PI32 next_vertex = iv - op_id;
            if ( next_vertex == prev_vertex )
                return;

            // fill the corresponding column
            for( int d = 0; d < nb_dims; ++d )
                M[ d ][ c - 1 ] = vertex_coords[ next_vertex ][ d ] - vertex_coords[ prev_vertex ][ d ];

            // recursion
            add_measure_rec( res, M, next_num_cuts, next_vertex, op_id, measure_for_each_cut );
        } );
    } else {
        res += abs( determinant( M ) );
    }
}

DTP void UTP::add_measure_rec( auto &res, auto &M, const auto &num_cuts, PI32 prev_vertex, PI op_id ) const {
    using std::abs;

    if constexpr ( constexpr PI c = num_cuts.ct_size ) {
        CtRange<0,c>::for_each_item( [&]( auto n ) {
            // next item ref
            auto next_num_cuts = num_cuts.without_index( n );

            // vertex choice for this item
            auto &iv = num_cut_map[ CtInt<c-1>() ].map[ next_num_cuts ];
            if ( iv < op_id ) {
                iv = op_id + prev_vertex;
                return;
            }

            //
            const PI32 next_vertex = iv - op_id;
            if ( next_vertex == prev_vertex )
                return;

            // fill the corresponding column
            for( int d = 0; d < nb_dims; ++d )
                M[ d ][ c - 1 ] = vertex_coords[ next_vertex ][ d ] - vertex_coords[ prev_vertex ][ d ];

            // recursion
            add_measure_rec( res, M, next_num_cuts, next_vertex, op_id );
        } );
    } else {
        res += abs( determinant( M ) );
    }
}

DTP typename UTP::TF UTP::measure() const {
    num_cut_map.for_each_item( [&]( auto &obj ) { obj.map.prepare_for( cuts.size() ); } );
    PI op_id = new_cut_oid( nb_vertices() );

    TF res = 0;
    Vec<Vec<TF,nb_dims>,nb_dims> M;
    for( PI n = 0; n < nb_vertices(); ++n )
        add_measure_rec( res, M, vertex_cuts[ n ].inds, n, op_id );

    TF coe = 1;
    for( int d = 2; d <= nb_dims; ++d )
        coe *= d;

    return res / coe;
}

DTP void UTP::display_vtk( VtkOutput &vo, const std::function<Vec<VtkOutput::TF,3>( const Pt &pt )> &to_vtk ) const { //
    const auto point_data = [&]( const auto &vertices ) -> std::map<std::string,typename VtkOutput::VTF> {
        typename VtkOutput::VTF convex_function;
        typename VtkOutput::VTF is_outside;
        Vec<VtkOutput::Pt> points;
        for( PI32 num_vertex : vertices ) {
            convex_function << conv( CtType<VtkOutput::TF>(), sp( vertex_coords[ num_vertex ], p0 ) - ( norm_2_p2( p0 ) - w0 ) / 2 );
            is_outside << any( vertex_cuts[ num_vertex ].inds, [&]( PI32 num_cut ) { return cuts[ num_cut ].is_inf(); } );
            points << to_vtk( vertex_coords[ num_vertex ] );
        }
        return { { "convex_function", convex_function }, { "is_outside", is_outside } };
    };

    const auto point_coords = [&]( const auto &vertices ) -> Vec<typename VtkOutput::Pt> {
        Vec<VtkOutput::Pt> res;
        for( PI32 num_vertex : vertices )
            res << to_vtk( vertex_coords[ num_vertex ] );
        return res;
    };

    // edges
    if constexpr ( nb_dims >= 1 ) {
        for_each_edge( [&]( Vec<PI,nb_dims-1> num_cuts, auto vertices ) {
            vo.add_edge( point_coords( vertices ), point_data( vertices ) );
        } );
    }

    // faces
    if constexpr ( nb_dims >= 2 ) {
        for_each_face( [&]( Vec<PI,nb_dims-2> bc, auto vertices ) {
            vo.add_polygon( point_coords( vertices ), point_data( vertices ) );
        } );
    }

    // polyhedra
    // if constexpr ( nb_dims >= 3 ) {
    //     for_each_polyhedron( [&]( Vec<PI,nb_dims-3> bc, auto vertices ) {
    //         vo.add_polyhedron( point_coords( vertex_indices ), point_data( vertices ) );
    //     } );
    // }
}

DTP void UTP::display_vtk( VtkOutput &vo ) const {
    return display_vtk( vo, []( const Pt &pos ) {
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

DTP bool UTP::contains( const Pt &x ) const {
    for( const auto &cut : cuts )
        if ( sp( cut.dir, x ) > cut.off )
            return false;
    return true;
}

DTP typename UTP::TF UTP::height( const Pt &point ) const {
    return sp( point, p0 ) - ( norm_2_p2( p0 ) - w0 ) / 2;
}

DTP bool UTP::empty() const {
    return nb_vertices() == 0;
}

#undef DTP
#undef UTP
