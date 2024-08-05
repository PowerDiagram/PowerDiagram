#include "support/operators/argmax.h"
#include "support/TODO.h"
#include "support/P.h"

#include "PointTree_AABB.h"

#define DTP template<class Scalar,int nb_dims>
#define UTP PointTree_AABB<Scalar,nb_dims>

DTP UTP::PointTree_AABB( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree<Scalar,nb_dims> *parent, PI num_in_parent ) : PointTree<Scalar,nb_dims>( points, weights, indices, parent, num_in_parent ) {
    init_bounds( cp );
    init_children( cp );
}

#ifndef AVOID_DISPLAY
DTP DisplayItem *UTP::display( DisplayItemFactory &ds ) const {
    const auto &children = this->children;
    if ( children.size() )
        return DS_OBJECT( PointTree_AABB, min_pos, max_pos, children );

    const auto &weights = this->weights;
    const auto &points = this->points;
    return DS_OBJECT( PointTree_AABB, min_pos, max_pos, points, weights );
}
#endif

DTP void UTP::init_children( const PointTreeCtorParms &cp ) {
    const PI n = this->points.size();
    if ( n <= cp.max_nb_points ) {
        // if we have cells sharing the same point, we keep the one with the largest weight
        PI k = 0;
        for( PI i = 0; i < this->indices.size(); ++i ) {
            for( PI j = 0; ; ++j ) {
                // no similar point
                if ( j == i ) {
                    if ( k != i ) {
                        this->indices[ k ] = this->indices[ i ];
                        this->weights[ k ] = this->weights[ i ];
                        this->points[ k ] = this->points[ i ];
                    }
                    ++k;
                    break;
                }
                // similar point
                if ( all( this->points[ j ] == this->points[ i ] ) ) {
                    if ( this->weights[ j ] < this->weights[ i ] ) {
                        this->indices[ j ] = this->indices[ i ];
                        this->weights[ j ] = this->weights[ i ];
                    }
                    break;
                }
            }        
        }

        this->indices.resize( k );
        this->weights.resize( k );
        this->points.resize( k );

        return;
    }

    // inplace swap
    PI dd = argmax( max_pos - min_pos );
    Scalar sep = ( max_pos[ dd ] + min_pos[ dd ] ) / 2;
    PI i = 0;
    for( PI j = this->points.size() - 1; ; ) {
        // while we have points at the right place
        while ( this->points[ i ][ dd ] <= sep )
            ++i;
        while ( this->points[ j ][ dd ] > sep )
            --j;
        if ( j < i )
            break;

        // swap the 2 points that are at the wrong places
        std::swap( this->indices[ i ], this->indices[ j ] );
        std::swap( this->weights[ i ], this->weights[ j ] );
        std::swap( this->points[ i ], this->points[ j ] );
        ++i;
        --j;
        if ( j < i )
            break;
    }

    // create 2 children
    this->children << new PointTree_AABB( cp, this->points.subspan( 0, i ), this->weights.subspan( 0, i ), this->indices.subspan( 0, i ), this, 0 );
    this->children << new PointTree_AABB( cp, this->points.subspan( i ), this->weights.subspan( i ), this->indices.subspan( i ), this, 1 );
}

DTP void UTP::init_bounds( const PointTreeCtorParms &cp ) {
    const PI n = this->points.size();
    if ( n == 0 )
        return;

    min_offset_weights = this->weights[ 0 ];
    max_offset_weights = this->weights[ 0 ];
    coeff_weights = { FromItemValue(), 0 };

    min_pos = this->points[ 0 ];
    max_pos = this->points[ 0 ];
    for( PI i = 1; i < n; ++i ) {
        for( PI d = 0; d < nb_dims; ++d ) {
            min_pos[ d ] = min( min_pos[ d ], this->points[ i ][ d ] );
            max_pos[ d ] = max( max_pos[ d ], this->points[ i ][ d ] );
        }

        min_offset_weights = min( min_offset_weights, this->weights[ i ] );
        max_offset_weights = max( max_offset_weights, this->weights[ i ] );
    }
}

DTP Str UTP::type_name() {
    return "PointTree_AABB";
}

#undef DTP
#undef UTP
