#include "support/operators/norm_2.h"
#include "support/operators/sp.h"
#include "support/compare.h"
// #include "support/P.h"
#include "Cell.h"

#include <eigen3/Eigen/LU>

void Cell::init_geometry_from( const Cell &that ) {
    vertices = that.vertices;
    edges = that.edges;
    cuts = that.cuts;
}

void Cell::make_init_simplex( const Point &center, Scalar radius ) {
    vertices.clear();
    edges.clear();
    cuts.clear();

    // cuts
    SI point_index = 0;
    for( int d = 0; d < PD_DIM; ++d ) {
        Point dir( FromItemValue(), 0 );
        dir[ d ] = -1;
        cuts.push_back( --point_index, dir, radius - center[ d ] );
    }

    Point dir( FromItemValue(), 1 );
    Point vrd( FromItemValue(), radius );
    cuts.push_back( --point_index, dir, sp( center + vrd, dir ) );

    // vertices
    for( int nc_0 = 0; nc_0 < PD_DIM + 1; ++nc_0 ) {
        Vec<PI,PD_DIM> num_cuts;
        for( int i = 0; i < nc_0; ++i )
            num_cuts[ i ] = i;
        for( int i = nc_0 + 1; i < PD_DIM + 1; ++i )
            num_cuts[ i - 1 ] = i;

        vertices.push_back( num_cuts, compute_pos( num_cuts ) );
    }

    // edges
    for( int nc_0 = 0; nc_0 < PD_DIM; ++nc_0 ) {
        Vec<PI,PD_DIM-1> num_cuts;
        for( int i = 0; i < nc_0; ++i )
            num_cuts[ i ] = i;

        for( int nc_1 = nc_0 + 1; nc_1 < PD_DIM + 1; ++nc_1 ) {
            for( int i = nc_0 + 1; i < nc_1; ++i )
                num_cuts[ i - 1 ] = i;
            for( int i = nc_1 + 1; i < PD_DIM + 1; ++i )
                num_cuts[ i - 2 ] = i;

            edges.push_back( num_cuts, Vec<PI,2>{ PI( nc_0 ), PI( nc_1 ) } );
        }
    }
}

TTi auto Cell::array_without_index( const Vec<T,i> &values, PI index ) {
    Vec<T,i-1> res;
    for( int d = 0, o = 0; d < i; ++d )
        if ( d != index )
            res[ o++ ] = values[ d ];
    return res;
}


TTi auto Cell::array_with_value( const Vec<T,i> &a, T value ) {
    Vec<T,i+1> res;
    for( PI n = 0; n < i; ++n )
        res[ n ] = a[ n ];
    res[ i ] = value;
    return res;
}

TT void Cell::apply_corr( Vec<T> &vec, Vec<int> &keep ) {
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

Point Cell::compute_pos( const Point &p0, const Point &p1, Scalar s0, Scalar s1 ) const {
    return p0 - s0 / ( s1 - s0 ) * ( p1 - p0 );
}

Point Cell::compute_pos( Vec<PI,PD_DIM> num_cuts ) const {
    using TM = Eigen::Matrix<Scalar,PD_DIM,PD_DIM>;
    using TV = Eigen::Matrix<Scalar,PD_DIM,1>;

    TM m;
    TV v;
    for( PI i = 0; i < PD_DIM; ++i ) {
        for( PI j = 0; j < PD_DIM; ++j )
            m( i, j ) = cuts[ num_cuts[ i ] ].dir[ j ];
        v( i ) = cuts[ num_cuts[ i ] ].sp;
    }

    Eigen::PartialPivLU<TM> lu( m );
    TV x = lu.solve( v );

    Point res;
    for( PI i = 0; i < PD_DIM; ++i )
        res[ i ] = x[ i ];

    return res;
}

void Cell::cut( const Point &dir, Scalar off, SI point_index ) {
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
    if constexpr ( PD_DIM >= 2 )
        waiting_vertices.init( cuts.size(), -1 );

    // for each edge
    edge_corr.resize( edges.size() );
    for( PI num_edge = 0, nb_edges = edges.size(); num_edge < nb_edges; ++num_edge ) {
        Edge *edge = &edges[ num_edge ];

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
            if constexpr ( PD_DIM >= 2 )
                for( int d = 0; d < PD_DIM - 1; ++d )
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
            if constexpr ( PD_DIM >= 2 )
                for( int d = 0; d < PD_DIM - 1; ++d )
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
    for( Edge &edge : edges )
        for( PI i = 0; i < 2; ++i )
            edge.vertices[ i ] = vertex_corr[ edge.vertices[ i ] ];
}

void Cell::for_each_vertex( const std::function<void( const Vertex &v )> &f ) const {
    for( const Vertex &v : vertices )
        f( v );
}

void Cell::for_each_face( const std::function<void( Vec<PI,PD_DIM-2> num_cuts, Span<const Vertex *> vertices )> &f ) const {
    // face => edge list
    std::map<Vec<PI,PD_DIM-2>,Vec<Vec<const Vertex *,2>>,Less> map;
    for_each_edge( [&]( Vec<PI,PD_DIM-1> bc, const Vertex *v0, const Vertex *v1 ) {
        for( PI nf = 0; nf < PD_DIM - 1; ++nf ) {
            Vec<PI,PD_DIM-2> bd = array_without_index( bc, nf );
            map[ bd ].push_back( Vec<const Vertex *,2>{ v0, v1 } );
        }
    } );
    if ( map.empty() )
        return;

    // for each face
    Vec<const Vertex *> vertices;
    for( const auto &face : map ) {
        vertices.clear();
        ++curr_op_id;

        // helper to find an unused vertex
        auto find_unused_vertex = [&]() -> const Vertex * {
            for( const auto &a : face.second )
                if ( a[ 0 ]->op_id != curr_op_id )
                    return a[ 0 ];
            return nullptr;
        };

        // make edge sweep. TODO: avoid this O(n^2) behavior
        while ( const Vertex *p = find_unused_vertex() ) {
            vertices.push_back( p );
            p->op_id = curr_op_id;

            // helper to find the next vertex in the face
            auto find_next_vertex = [&]() -> const Vertex * {
                for( const auto &a : face.second ) {
                    if ( a[ 0 ] == p && a[ 1 ]->op_id != curr_op_id )
                        return a[ 1 ];
                    if ( a[ 1 ] == p && a[ 0 ]->op_id != curr_op_id )
                        return a[ 0 ];
                }
                return nullptr;
            };

            //
            while ( const Vertex *n = find_next_vertex() ) {
                vertices.push_back( n );
                n->op_id = curr_op_id;
                p = n;
            }

            //
            f( face.first, vertices );
        }
    }
}

void Cell::for_each_edge( const std::function<void( Vec<PI,PD_DIM-1> num_cuts, const Vertex *v0, const Vertex *v1 )> &f ) const {
    for( const Edge &e : edges )
        f( e.num_cuts, vertices.data() + e.vertices[ 0 ], vertices.data() + e.vertices[ 1 ] );
}

bool Cell::vertex_has_cut( const Vertex &vertex, const std::function<bool( SI point_index )> &outside_cut ) const {
    for( auto num_cut : vertex.num_cuts )
        if ( outside_cut( cuts[ num_cut ].n_index ) )
            return true;
    return false;
}

void Cell::display_vtk( VtkOutput &vo, const std::function<void( VtkOutput::Pt &pt )> &coord_change ) const { //
    auto to_vtk = [&]( const auto &pos ) {
        VtkOutput::Pt res;
        for( PI i = 0; i < min( PI( pos.size() ), res.size() ); ++i )
            res[ i ] = pos[ i ];
        for( PI i = PI( pos.size() ); i < res.size(); ++i )
            res[ i ] = 0;
        coord_change( res );
        return res;
    };

    auto add_item = [&]( int vtk_id, Span<const Vertex *> vertices ) {
        Vec<VtkOutput::Pt> points;
        VtkOutput::VTF convex_function;
        VtkOutput::VTF is_outside;
        for( const Vertex *vertex : vertices ) {
            convex_function << sp( vertex->pos, *orig_point ) - ( norm_2_p2( *orig_point ) - *orig_weight ) / 2;
            is_outside << vertex_has_cut( *vertex, []( SI v ) { return v < 0; } );
            points << to_vtk( vertex->pos );
        }
        vo.add_polygon( points, { { "convex_function", convex_function }, { "is_outside", is_outside } } );
    };

    // edges
    if constexpr ( PD_DIM >= 1 ) {
        for_each_edge( [&]( Vec<PI,PD_DIM-1> num_cuts, const Vertex *v0, const Vertex *v1 ) {
            const Vertex *vs[] = { v0, v1 };
            add_item( VtkOutput::VtkLine, { vs, 2 } );
        } );
    }

    // faces
    if constexpr ( PD_DIM >= 2 ) {
        for_each_face( [&]( Vec<PI,PD_DIM-2> bc, Span<const Vertex *> vertices ) {
            add_item( VtkOutput::VtkPolygon, vertices );
        } );
    }
}

void Cell::display_vtk( VtkOutput &vo ) const {
    return display_vtk( vo, []( const Point &v ) { return v; } );
}
