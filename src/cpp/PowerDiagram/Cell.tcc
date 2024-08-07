#pragma once

#include "support/operators/norm_2.h"
#include "support/operators/sp.h"
#include "support/compare.h"
#include "support/conv.h"
#include "Cell.h"

#include <eigen3/Eigen/LU>

#define DTP template<class Scalar,int nb_dims>
#define UTP Cell<Scalar,nb_dims>

DTP void UTP::init_geometry_from( const Cell &that ) {
    vertices = that.vertices;
    edges = that.edges;
    cuts = that.cuts;
}

DTP void UTP::make_init_simplex( const Point &mi, const Point &ma ) {
    Point min_pos = ( mi + ma ) / 2 - ( ma - mi );
    Point max_pos = ( mi + ma ) / 2 + ( ma - mi );

    vertices.clear();
    edges.clear();
    cuts.clear();

    // cuts
    SI point_index = 0;
    for( int d = 0; d < nb_dims; ++d ) {
        Point dir( FromItemValue(), 0 );
        dir[ d ] = -1;
        cuts.push_back( --point_index, dir, sp( min_pos, dir ) );
    }

    Point dir( FromItemValue(), 1 );
    cuts.push_back( --point_index, dir, sp( max_pos, dir ) );

    // vertices
    for( int nc_0 = 0; nc_0 < nb_dims + 1; ++nc_0 ) {
        Vec<PI,nb_dims> num_cuts( FromItemValue(), 0 );
        for( int i = 0; i < nc_0; ++i )
            num_cuts[ i ] = i;
        for( int i = nc_0 + 1; i < nb_dims + 1; ++i )
            num_cuts[ i - 1 ] = i;

        vertices.push_back( num_cuts, compute_pos( num_cuts ) );
    }

    // edges
    for( int nc_0 = 0; nc_0 < nb_dims; ++nc_0 ) {
        Vec<PI,nb_dims-1> num_cuts;
        for( int i = 0; i < nc_0; ++i )
            num_cuts[ i ] = i;

        for( int nc_1 = nc_0 + 1; nc_1 < nb_dims + 1; ++nc_1 ) {
            for( int i = nc_0 + 1; i < nc_1; ++i )
                num_cuts[ i - 1 ] = i;
            for( int i = nc_1 + 1; i < nb_dims + 1; ++i )
                num_cuts[ i - 2 ] = i;

            edges.push_back( num_cuts, Vec<PI,2>{ PI( nc_0 ), PI( nc_1 ) } );
        }
    }
}

DTP TTi auto UTP::array_without_index( const Vec<T,i> &values, PI index ) {
    Vec<T,i-1> res;
    for( int d = 0, o = 0; d < i; ++d )
        if ( PI( d ) != index )
            res[ o++ ] = values[ d ];
    return res;
}


DTP TTi auto UTP::array_with_value( const Vec<T,i> &a, T value ) {
    Vec<T,i+1> res;
    for( PI n = 0; n < i; ++n )
        res[ n ] = a[ n ];
    res[ i ] = value;
    return res;
}

DTP TT void UTP::apply_corr( Vec<T> &vec, Vec<int> &keep ) {
    int last_keep = vec.size();
    for( int i = 0; i < last_keep; ++i ) {
        if ( keep[ i ] ) {
            keep[ i ] = i;
            continue;
        }

        while( --last_keep > i && ! keep[ last_keep ] )
            keep[ last_keep ] = -1;

        vec[ i ] = std::move( vec[ last_keep ] );
        keep[ last_keep ] = i;
        keep[ i ] = -1;
    }

    vec.resize( last_keep );
}

DTP UTP::Point UTP::compute_pos( const Point &p0, const Point &p1, Scalar s0, Scalar s1 ) const {
    return p0 - s0 / ( s1 - s0 ) * ( p1 - p0 );
}

DTP UTP::Point UTP::compute_pos( Vec<PI,nb_dims> num_cuts ) const {
    if constexpr ( nb_dims == 0 ) {
        return {};
    } else {
        using TM = Eigen::Matrix<Scalar,nb_dims,nb_dims>;
        using TV = Eigen::Matrix<Scalar,nb_dims,1>;

        TM m;
        TV v;
        for( PI i = 0; i < nb_dims; ++i ) {
            for( PI j = 0; j < nb_dims; ++j )
                m( i, j ) = cuts[ num_cuts[ i ] ].dir[ j ];
            v( i ) = cuts[ num_cuts[ i ] ].sp;
        }

        Eigen::PartialPivLU<TM> lu( m );
        TV x = lu.solve( v );

        Point res;
        for( PI i = 0; i < nb_dims; ++i )
            res[ i ] = x[ i ];

        return res;
    }
}

DTP void UTP::cut( const Point &dir, Scalar off, SI point_index ) {
    // scalar product for each vertex
    vertex_corr.resize( vertices.size() );
    sps.resize( vertices.size() );
    bool has_ext = false;
    for( PI num_vertex = 0; num_vertex < vertices.size(); ++num_vertex ) {
        Scalar ext = sp( vertices[ num_vertex ].pos, dir ) - off;
        vertex_corr[ num_vertex ] = ! ( ext > 0 );
        sps[ num_vertex ] = ext;
        has_ext |= ext > 0;
    }

    // all int ?
    if ( ! has_ext )
        return;

    // add the new cut
    PI new_cut = cuts.size();
    cuts.push_back( point_index, dir, off );

    //
    if constexpr ( nb_dims >= 2 )
        waiting_vertices.init( cuts.size(), -1 );

    // for each edge
    edge_corr.resize( edges.size() );
    for( PI num_edge = 0, nb_edges = edges.size(); num_edge < nb_edges; ++num_edge ) {
        Edge<Scalar,nb_dims> *edge = &edges[ num_edge ];

        // helper to create the new edges (on faces that have a cut)
        auto add_to_waiting_vertices = [&]( auto face, PI vertex ) {
            int &wv = waiting_vertices[ face ];
            if ( wv >= 0 ) {
                edges.push_back( array_with_value( face, new_cut ), Vec<PI,2>{ PI( wv ), vertex } );
                edge = &edges[ num_edge ];
                wv = -1;
            } else
                wv = vertex;
        };

        // all ext => remove it
        const PI o0 = edge->vertices[ 0 ];
        const PI o1 = edge->vertices[ 1 ];
        const Scalar s0 = sps[ o0 ];
        const Scalar s1 = sps[ o1 ];
        const bool e0 = s0 > 0;
        const bool e1 = s1 > 0;
        if ( e0 && e1 ) {
            edge_corr[ num_edge ] = 0;
            continue;
        }

        // => we're going to keep this edge (potentially with a modification)
        edge_corr[ num_edge ] = 1;

        // only v0 is ext
        if ( e0 ) {
            const PI nv = vertices.size();
            edge->vertices[ 0 ] = nv;

            // Point new_pos = ;
            Point new_pos = compute_pos( vertices[ o0 ].pos, vertices[ o1 ].pos, s0, s1 );
            auto num_cuts = array_with_value( edge->num_cuts, new_cut );
            vertices.push_back( num_cuts, new_pos );

            // add a waiting vertex for each face
            if constexpr ( nb_dims >= 2 )
                for( int d = 0; d < nb_dims - 1; ++d )
                    add_to_waiting_vertices( array_without_index( edge->num_cuts, d ), nv );

            continue;
        }

        // only v1 is ext
        if ( e1 ) {
            const PI nv = vertices.size();
            edge->vertices[ 1 ] = nv;

            Point new_pos = compute_pos( vertices[ o0 ].pos, vertices[ o1 ].pos, s0, s1 );
            auto num_cuts = array_with_value( edge->num_cuts, new_cut );
            vertices.push_back( num_cuts, new_pos );

            // add a waiting vertex for each face
            if constexpr ( nb_dims >= 2 )
                for( int d = 0; d < nb_dims - 1; ++d )
                    add_to_waiting_vertices( array_without_index( edge->num_cuts, d ), nv );

            continue;
        }
    }


    // move vertices to the new positions
    while ( vertex_corr.size() < vertices.size() )
        vertex_corr.push_back( 1 );
    apply_corr( vertices, vertex_corr );

    // move edges to the new positions
    while ( edge_corr.size() < edges.size() )
        edge_corr.push_back( 1 );
    apply_corr( edges, edge_corr );

    // update vertex refs
    for( Edge<Scalar,nb_dims> &edge : edges )
        for( PI i = 0; i < 2; ++i )
            edge.vertices[ i ] = vertex_corr[ edge.vertices[ i ] ];
}

DTP void UTP::for_each_vertex( const std::function<void( const Vertex<Scalar,nb_dims> &v )> &f ) const {
    for( const Vertex<Scalar,nb_dims> &v : vertices )
        f( v );
}

DTP void UTP::for_each_face( const std::function<void( Vec<PI,nb_dims-2> num_cuts, Span<const Vertex<Scalar,nb_dims> *> vertices )> &f ) const {
    // face => edge list
    std::map<Vec<PI,nb_dims-2>,Vec<Vec<const Vertex<Scalar,nb_dims> *,2>>,Less> map;
    for_each_edge( [&]( Vec<PI,nb_dims-1> bc, const Vertex<Scalar,nb_dims> &v0, const Vertex<Scalar,nb_dims> &v1 ) {
        for( PI nf = 0; nf < nb_dims - 1; ++nf ) {
            Vec<PI,nb_dims-2> bd = array_without_index( bc, nf );
            map[ bd ].push_back( Vec<const Vertex<Scalar,nb_dims> *,2>{ &v0, &v1 } );
        }
    } );
    if ( map.empty() )
        return;

    // for each face
    Vec<const Vertex<Scalar,nb_dims> *> vertices;
    for( const auto &face : map ) {
        vertices.clear();
        ++curr_op_id;

        // helper to find an unused vertex
        auto find_unused_vertex = [&]() -> const Vertex<Scalar,nb_dims> * {
            for( const auto &a : face.second )
                if ( a[ 0 ]->op_id != curr_op_id )
                    return a[ 0 ];
            return nullptr;
        };

        // make edge sweep. TODO: avoid this O(n^2) behavior
        while ( const Vertex<Scalar,nb_dims> *p = find_unused_vertex() ) {
            vertices.push_back( p );
            p->op_id = curr_op_id;

            // helper to find the next vertex in the face
            auto find_next_vertex = [&]() -> const Vertex<Scalar,nb_dims> * {
                for( const auto &a : face.second ) {
                    if ( a[ 0 ] == p && a[ 1 ]->op_id != curr_op_id )
                        return a[ 1 ];
                    if ( a[ 1 ] == p && a[ 0 ]->op_id != curr_op_id )
                        return a[ 0 ];
                }
                return nullptr;
            };

            //
            while ( const Vertex<Scalar,nb_dims> *n = find_next_vertex() ) {
                vertices.push_back( n );
                n->op_id = curr_op_id;
                p = n;
            }

            //
            f( face.first, vertices );
        }
    }
}

DTP void UTP::for_each_edge( const std::function<void( Vec<PI,nb_dims-1> num_cuts, const Vertex<Scalar,nb_dims> &v0, const Vertex<Scalar,nb_dims> &v1 )> &f ) const {
    for( const Edge<Scalar,nb_dims> &e : edges )
        f( e.num_cuts, vertices[ e.vertices[ 0 ] ], vertices[ e.vertices[ 1 ] ] );
}

DTP bool UTP::vertex_has_cut( const Vertex<Scalar,nb_dims> &vertex, const std::function<bool( SI point_index )> &outside_cut ) const {
    for( auto num_cut : vertex.num_cuts )
        if ( outside_cut( cuts[ num_cut ].n_index ) )
            return true;
    return false;
}

DTP void UTP::add_measure_rec( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex ) const {
    if constexpr ( constexpr PI c = decltype( num_cuts.size() )::value ) {
        for( int n = 0; n < num_cuts.size(); ++n ) {
            // next item ref
            auto next_num_cuts = num_cuts.without_index( n );

            // vertex choice for this item
            int &next_vertex = item_to_vertex[ next_num_cuts ];
            if ( next_vertex < 0 ) {
                next_vertex = last_vertex;
                continue;
            }

            // matrix column
            const auto &last_pos = vertices[ last_vertex ].pos;
            const auto &next_pos = vertices[ next_vertex ].pos;
            for( int d = 0; d < nb_dims; ++d )
                M.coeffRef( d, c - 1 ) = next_pos[ d ] - last_pos[ d ];

            // recursion
            add_measure_rec( res, M, item_to_vertex, next_num_cuts, next_vertex );
        }
    } else {
        using std::abs;
        res += abs( M.determinant() );
    }
}

DTP Scalar UTP::measure() const {
    MapOfUniquePISortedArray<PI,0,nb_dims-1,int> item_to_vertex;
    item_to_vertex.init( cuts.size(), -1 );

    Scalar res = 0;
    Eigen::Matrix<Scalar,nb_dims,nb_dims> M;
    for( PI i = 0; i < vertices.size(); ++i ) {
        auto num_cuts = vertices[ i ].num_cuts;
        std::sort( num_cuts.begin(), num_cuts.end() );
        add_measure_rec( res, M, item_to_vertex, num_cuts, i );
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

    auto add_item = [&]( int vtk_id, Span<const Vertex<Scalar,nb_dims> *> vertices ) {
        Vec<VtkOutput::Pt> points;
        VtkOutput::VTF convex_function;
        VtkOutput::VTF is_outside;
        for( const Vertex<Scalar,nb_dims> *vertex : vertices ) {
            if ( orig_point && orig_weight )
                convex_function << conv( CtType<VtkOutput::TF>(), sp( vertex->pos, *orig_point ) - ( norm_2_p2( *orig_point ) - *orig_weight ) / 2 );
            is_outside << vertex_has_cut( *vertex, []( SI v ) { return v < 0; } );
            points << to_vtk( vertex->pos );
        }
        if ( orig_point && orig_weight )
            vo.add_polygon( points, { { "convex_function", convex_function }, { "is_outside", is_outside } } );
        else
            vo.add_polygon( points, { { "is_outside", is_outside } } );
    };

    // edges
    if constexpr ( nb_dims >= 1 ) {
        for_each_edge( [&]( Vec<PI,nb_dims-1> num_cuts, const Vertex<Scalar,nb_dims> &v0, const Vertex<Scalar,nb_dims> &v1 ) {
            const Vertex<Scalar,nb_dims> *vs[] = { &v0, &v1 };
            add_item( VtkOutput::VtkLine, { vs, 2 } );
        } );
    }

    // faces
    if constexpr ( nb_dims >= 2 ) {
        for_each_face( [&]( Vec<PI,nb_dims-2> bc, Span<const Vertex<Scalar,nb_dims> *> vertices ) {
            add_item( VtkOutput::VtkPolygon, vertices );
        } );
    }
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
        used_fs[ orig_index ] = true;
    for_each_vertex( [&]( const Vertex<Scalar,nb_dims> &v ) {
        for( PI num_cut : v.num_cuts ) {
            SI ind = cuts[ num_cut ].n_index;
            if ( ind >= SI( nb_bnds ) )
                used_fs[ ind - nb_bnds ] = true;
            else if ( ind >= 0 )
                used_bs[ ind ] = true;
        }
    } );
}

DTP bool UTP::has_inf_cut( const Vertex<Scalar,nb_dims> &vertex ) const {
    for( const PI num_cut : vertex.num_cuts )
        if ( cuts[ num_cut ].n_index < 0 )
            return true;
    return false;
}

DTP bool UTP::is_inf() const {
    if ( vertices.empty() )
        return true;
    for( const Vertex<Scalar,nb_dims> &vertex : vertices )
        if ( has_inf_cut( vertex ) )
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
    return sp( point, *orig_point ) - ( norm_2_p2( *orig_point ) - *orig_weight ) / 2;
}

DTP bool UTP::empty() const {
    return vertices.empty();
}

#undef DTP
#undef UTP
