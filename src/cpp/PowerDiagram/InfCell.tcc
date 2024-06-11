#pragma once

#include "support/for_each_selection.h"
#include "support/operators/sp.h"
#include "support/TODO.h"

#include "support/P.h"

#include <eigen3/Eigen/LU>

#include "InfCell.h"

#define DTP template<class Scalar,int nb_dims>
#define UTP InfCell<Scalar,nb_dims>

DTP void UTP::cut( const Point &dir, Scalar off, SI point_index ) {
    // remove vertices that are outside the cut
    for( PI num_vertex = 0; num_vertex < vertices.size(); ++num_vertex ) {
        if ( sp( vertices[ num_vertex ].pos, dir ) > off ) {
            auto repl = vertices.pop_back_val();
            vertices[ num_vertex-- ] = repl;
        }
    }

    // add the new cut
    PI new_cut = cuts.push_back_ind( point_index, dir, off );

    // create the new vertices (from all the new cut combinations)
    if ( new_cut >= nb_dims - 1 ) {
        for_each_selection( [&]( const Vec<int> &selection_of_cuts ) {
            // get the new coordinates
            Vec<PI,nb_dims> num_cuts;
            for( PI i = 0; i < nb_dims - 1; ++i )
                num_cuts[ i ] = selection_of_cuts[ i ];
            num_cuts[ nb_dims - 1 ] = new_cut;

            // early return if parallel cuts
            Opt<Point> pos = compute_pos( num_cuts );
            if ( ! pos )
                return;

            // early return if the new vertex is outside
            for( PI num_cut = 0; num_cut < new_cut; ++num_cut )
                if ( selection_of_cuts.contains( int( num_cut ) ) == false && sp( *pos, cuts[ num_cut ].dir ) > cuts[ num_cut ].sp )
                    return;

            // else, register the new vertex
            vertices.push_back( num_cuts, *pos );
        }, nb_dims - 1, new_cut );
    }

    //
    clean_inactive_cuts();
}

DTP void UTP::clean_inactive_cuts() {
    // mark cuts used by actives vertices
    Vec<int> keep( FromSizeAndItemValue(), cuts.size(), 0 );
    for( const Vertex<Scalar,nb_dims> &vertex : vertices )
        for( PI num_cut : vertex.num_cuts )
            keep[ num_cut ] = true;

    // "inactive" cuts may actually be used
    for( PI num_cut = 0; num_cut < cuts.size(); ++num_cut )
        if ( ! keep[ num_cut ] )
            keep[ num_cut ] = cut_is_useful( num_cut );

    // update the cut list
    apply_corr( cuts, keep );

    // update the vertex list
    for( Vertex<Scalar,nb_dims> &vertex : vertices )
        for( PI &num_cut : vertex.num_cuts )
            num_cut = keep[ num_cut ];
}

DTP bool UTP::cut_is_useful( PI num_cut ) {
    using TM = Eigen::Matrix<Scalar,Eigen::Dynamic,Eigen::Dynamic>;
    using TV = Eigen::Matrix<Scalar,Eigen::Dynamic,1>;

    Vec<PI> constraints;
    auto get_prop_point = [&]() -> Point {
        TM m( nb_dims + constraints.size(), nb_dims + constraints.size() );
        TV v( nb_dims + constraints.size() );

        // exterior of cut[ num_cut ]
        for( PI i = 0; i < nb_dims; ++i ) {
            for( PI j = 0; j < nb_dims; ++j )
                m( i, j ) = cuts[ num_cut ].dir[ i ] * cuts[ num_cut ].dir[ j ];
            v( i ) = cuts[ num_cut ].dir[ i ] * ( cuts[ num_cut ].sp + 1 );
        }

        // constraints
        for( PI i = 0; i < constraints.size(); ++i ) {
            for( PI j = 0; j < nb_dims; ++j ) {
                m( nb_dims + i, j ) = cuts[ constraints[ i ] ].dir[ j ];
                m( j, nb_dims + i ) = cuts[ constraints[ i ] ].dir[ j ];
            }
            v( nb_dims + i ) = cuts[ constraints[ i ] ].sp;
        }

        // zeros
        for( PI i = 0; i < constraints.size(); ++i )
            for( PI j = 0; j < constraints.size(); ++j )
                m( nb_dims + i, nb_dims + j ) = 0;

        // solve
        Eigen::FullPivLU<TM> lu( m );
        return lu.solve( v );
    };


    //
    auto outside_for_cut = [&]( const Point &x ) -> Opt<PI> {
        for( PI n = 0; n < cuts.size(); ++n ) {
            if ( n == num_cut || constraints.contains( n ) )
                continue;
            if ( sp( x, cuts[ n ].dir ) > cuts[ n ].sp )
                return { n };
        }
        return {};
    };

    // find a point and add constraint until interior
    while ( true ) {
        // impossible to find a point at the exterior of the cut, we can say that this cut is not useful
        Point x = get_prop_point();
        if ( sp( x, cuts[ num_cut ].dir ) <= cuts[ num_cut ].sp )
            return false;

        // if we have to add a constraint, loop again
        if ( Opt<PI> n = outside_for_cut( x ) ) {
            constraints << *n;
            continue;
        }

        // else, we can say that this cut is useful
        return true;
    }
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

DTP Opt<typename UTP::Point> UTP::compute_pos( Vec<PI,nb_dims> num_cuts ) const {
    if constexpr( nb_dims == 0 ) {
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

        Eigen::FullPivLU<TM> lu( m );
        if ( lu.dimensionOfKernel() )
            return {};

        return lu.solve( v );
    }
}

DTP void UTP::for_each_vertex( const std::function<void( const Vertex<Scalar,nb_dims> &v )> &f ) const {
    for( const Vertex<Scalar,nb_dims> &v : vertices )
        f( v );
}

DTP void UTP::display_vtk( VtkOutput &vo, const std::function<void( VtkOutput::Pt &pt )> &coord_change ) const { //
    TODO;
}

DTP void UTP::display_vtk( VtkOutput &vo ) const {
    return display_vtk( vo, []( VtkOutput::Pt & ) {} );
}

DTP Scalar UTP::height( const Point &point ) const {
    return sp( point, *orig_point ) - ( norm_2_p2( *orig_point ) - *orig_weight ) / 2;
}

#undef DTP
#undef UTP
