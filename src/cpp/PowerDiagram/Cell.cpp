#include "support/operators/norm_2.h"
#include "support/operators/sp.h"
#include "Cell.h"

void Cell::init( const Point *orig_point, const Scalar *orig_weight, SI orig_index ) {
    this->orig_weight = orig_weight;
    this->orig_point = orig_point;
    this->orig_index = orig_index;
}

void Cell_2_double::cut( const Point &dir, Scalar off, SI point_index ) {
    // scalar product for each vertex
    sps.resize( vertices.size() );
    bool has_ext = false;
    for( PI num_vertex = 0; num_vertex < vertices.size(); ++num_vertex ) {
        Scalar ext = sp( vertices[ num_vertex ].pos, dir ) - off;
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
                edges.push_back( array_with_value( face, new_cut ), std::array<PI,2>{ PI( wv ), vertex } );
                edge = &edges[ num_edge ];
                wv = -1;
            } else
                wv = vertex;
        };

        // all ext => remove it
        bool e0 = sps[ edge->vertices[ 0 ] ] > 0;
        bool e1 = sps[ edge->vertices[ 1 ] ] > 0;
        if ( e0 && e1 ) {
            edge_corr[ num_edge ] = 0;
            continue;
        }

        // => we're going to keep this edge (potentially with a modification)
        edge_corr[ num_edge ] = 1;

        // only v0 is ext
        if ( e0 ) {
            edge->vertices[ 1 ] = vertex_corr[ edge->vertices[ 1 ] ];
            edge->vertices[ 0 ] = vertices.size();

            auto num_cuts = array_with_value( edge->num_cuts, new_cut );
            vertices.push_back( num_cuts, compute_pos( num_cuts ) );

            // add a waiting vertex for each face
            if constexpr ( nb_dims >= 2 )
                for( int d = 0; d < nb_dims - 1; ++d )
                    add_to_waiting_vertices( array_without_index( edge->num_cuts, d ), edge->vertices[ 0 ] );

            continue;
        }

        // only v1 is ext
        if ( e1 ) {
            edge->vertices[ 0 ] = vertex_corr[ edge->vertices[ 0 ] ];
            edge->vertices[ 1 ] = vertices.size();

            auto num_cuts = array_with_value( edge->num_cuts, new_cut );
            vertices.emplace_back( num_cuts, compute_pos( num_cuts ) );

            // add a waiting vertex for each face
            if constexpr ( nb_dims >= 2 )
                for( int d = 0; d < nb_dims - 1; ++d )
                    add_to_waiting_vertices( array_without_index( edge->num_cuts, d ), edge->vertices[ 1 ] );

            continue;
        }

        // => all int
        for( PI i = 0; i < 2; ++i )
            edge->vertices[ i ] = vertex_corr[ edge->vertices[ i ] ];
    }

    // move edges to the new positions
    while ( edge_corr.size() < edges.size() )
        edge_corr.push_back( 1 );
    apply_corr( edges, edge_corr );
}

