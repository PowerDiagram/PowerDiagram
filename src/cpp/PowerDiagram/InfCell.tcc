#pragma once

#include "support/for_each_selection.h"
#include <eigen3/Eigen/LU>
#include "InfCell.h"


#define DTP template<class Scalar,int nb_dims>
#define UTP InfCell<Scalar,nb_dims>

DTP void UTP::cut( const Point &dir, Scalar off, SI point_index ) {
    // remove ext vertices
    bool all_int = true;
    for( PI num_vertex = 0; num_vertex < vertices.size(); ++num_vertex ) {
        Scalar ext = sp( vertices[ num_vertex ].pos, dir ) - off;
        if ( ext > 0 ) {
            if ( num_vertex + 1 != vertices.size() )
                vertices[ num_vertex-- ] = vertices.pop_back_val();
            all_int = false;
        }
    }

    // all int ?
    if ( all_int && ! vertices.empty() )
        return;

    // add the new cut
    PI new_cut = cuts.push_back_ind( point_index, dir, off );

    // create the new vertices
    for_each_selection( [&]( const Vec<int> &selection_of_cuts ) {
        // get the new coordinates
        Vec<PI,nb_dims> num_cuts;
        for( PI i = 0; i < nb_dims - 1; ++i )
            num_cuts[ i ] = selection_of_cuts[ i ];
        num_cuts[ nb_dims - 1 ] = new_cut;

        Point pos = compute_pos( num_cuts );

        // return if outside
        for( PI num_cut = 0; num_cut < new_cut; ++num_cut ) {
            if ( selection_of_cuts.contains( num_cut ) )
                continue;
            Scalar ext = sp( pos, cuts[ num_cut ].dir ) - cuts[ num_cut ].sp;
            if ( ext > 0 )
                return;
        }

        // else, register the new vertex
        vertices.push_back( num_cuts, pos );
    }, nb_dims - 1, new_cut );

    //
    clean_inactive_cuts();
}

DTP void UTP::clean_inactive_cuts() {
    // find the active cuts
    Vec<int> keep( FromSizeAndItemValue(), cuts.size(), 0 );
    for( const Vertex<Scalar,nb_dims> &vertex : vertices )
        for( PI num_cut : vertex.num_cuts )
            keep[ num_cut ] = true;

    // update the cut list
    apply_corr( cuts, keep );

    // update the vertex list
    for( Vertex<Scalar,nb_dims> &vertex : vertices )
        for( PI &num_cut : vertex.num_cuts )
            num_cut = keep[ num_cut ];
}

DTP TTi auto UTP::array_without_index( const Vec<T,i> &values, PI index ) {
    Vec<T,i-1> res;
    for( int d = 0, o = 0; d < i; ++d )
        if ( d != index )
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

DTP UTP::Point UTP::compute_pos( Vec<PI,nb_dims> num_cuts ) const {
    using TM = Eigen::Matrix<Scalar,nb_dims,nb_dims>;
    using TV = Eigen::Matrix<Scalar,nb_dims,1>;

    TM m;
    TV v;
    for( PI i = 0; i < nb_dims; ++i ) {
        for( PI j = 0; j < nb_dims; ++j )
            m( i, j ) = cuts[ num_cuts[ i ] ].dir[ j ];
        v( i ) = cuts[ num_cuts[ i ] ].sp;
    }

    Eigen::FullPivLU<TM> lu( m );
    TV x = lu.solve( v );

    Point res;
    for( PI i = 0; i < nb_dims; ++i )
        res[ i ] = x[ i ];

    return res;
}

DTP void UTP::for_each_vertex( const std::function<void( const Vertex<Scalar,nb_dims> &v )> &f ) const {
    for( const Vertex<Scalar,nb_dims> &v : vertices )
        f( v );
}

DTP void UTP::display_vtk( VtkOutput &vo, const std::function<void( VtkOutput::Pt &pt )> &coord_change ) const { //
    auto to_vtk = [&]( const auto &pos ) {
        VtkOutput::Pt res;
        for( PI i = 0; i < min( PI( pos.size() ), PI( res.size() ) ); ++i )
            res[ i ] = pos[ i ];
        for( PI i = PI( pos.size() ); i < PI( res.size() ); ++i )
            res[ i ] = 0;
        coord_change( res );
        return res;
    };

    auto add_item = [&]( int vtk_id, Span<const Vertex<Scalar,nb_dims> *> vertices ) {
        Vec<VtkOutput::Pt> points;
        VtkOutput::VTF convex_function;
        VtkOutput::VTF is_outside;
        for( const Vertex<Scalar,nb_dims> *vertex : vertices ) {
            convex_function << sp( vertex->pos, *orig_point ) - ( norm_2_p2( *orig_point ) - *orig_weight ) / 2;
            is_outside << vertex_has_cut( *vertex, []( SI v ) { return v < 0; } );
            points << to_vtk( vertex->pos );
        }
        vo.add_polygon( points, { { "convex_function", convex_function }, { "is_outside", is_outside } } );
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
    return display_vtk( vo, []( VtkOutput::Pt & ) {} );
}

DTP Scalar UTP::height( const Point &point ) const {
    return sp( point, *orig_point ) - ( norm_2_p2( *orig_point ) - *orig_weight ) / 2;
}

#undef DTP
#undef UTP
