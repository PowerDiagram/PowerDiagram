#include "support/operators/argmax.h"
#include "support/TODO.h"
#include "support/P.h"

#include "PointTree_AABB.h"

#include <eigen3/Eigen/LU>

#define DTP template<class Scalar,int nb_dims>
#define UTP PointTree_AABB<Scalar,nb_dims>

DTP UTP::PointTree_AABB( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree<Scalar,nb_dims> *parent, PI num_in_parent ) : PointTree<Scalar,nb_dims>( points, weights, indices, parent, num_in_parent ) {
    num_sym = 0;
    
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

    // for now, we recompute the coefficient for each the boxes.
    // TODO optimization: use data from the children
    Eigen::Matrix<Scalar,nb_dims+1,nb_dims+1> M;
    Eigen::Matrix<Scalar,nb_dims+1,1> V;
    for( PI r = 0; r <= nb_dims; ++r )
        for( PI c = 0; c <= nb_dims; ++c )
            M.coeffRef( r, c ) = 0;
    for( PI r = 0; r <= nb_dims; ++r )
        V[ r ] = 0;
    auto add_pt = [&]( const Point &point, const Scalar &weight ) {
        Scalar coeffs[ nb_dims + 1 ];
        for( PI r = 0; r < nb_dims; ++r )
            coeffs[ r ] = point[ r ];
        coeffs[ nb_dims ] = 1;

        for( PI r = 0; r <= nb_dims; ++r ) {
            for( PI c = 0; c <= nb_dims; ++c )
                M.coeffRef( r, c ) += coeffs[ r ] * coeffs[ c ];
            V[ r ] += coeffs[ r ] * weight;
        }
    };


    // points sweep
    min_pos = this->points[ 0 ];
    max_pos = this->points[ 0 ];
    add_pt( this->points[ 0 ], this->weights[ 0 ] );
    for( PI i = 1; i < n; ++i ) {
        for( PI d = 0; d < nb_dims; ++d ) {
            min_pos[ d ] = min( min_pos[ d ], this->points[ i ][ d ] );
            max_pos[ d ] = max( max_pos[ d ], this->points[ i ][ d ] );
        }
        add_pt( this->points[ i ], this->weights[ i ] );
    }

    // coefficients
    Eigen::FullPivLU<Eigen::Matrix<Scalar,nb_dims+1,nb_dims+1>> lu( M );
    auto X = lu.solve( V );
    for( PI d = 0; d < nb_dims; ++d )
        coeff_weights[ d ] = X[ d ];

    min_offset_weights = this->weights[ 0 ] - sp( coeff_weights, this->points[ 0 ] );
    max_offset_weights = min_offset_weights;

    for( PI i = 1; i < n; ++i ) {
        Scalar value = this->weights[ i ] - sp( coeff_weights, this->points[ i ] );
        min_offset_weights = min( min_offset_weights, value );
        max_offset_weights = max( max_offset_weights, value );
    }
}

DTP bool UTP::may_intersect( const Point &vertex, const Point &p0, Scalar w0 ) const {
    Point q1 = min( max_pos, max( min_pos, vertex + Scalar( 1 ) / 2 * coeff_weights ) );
    Point p1 = inv_sym( q1, this->num_sym );
    return norm_2_p2( vertex - p0 ) - w0 > norm_2_p2( vertex - p1 ) - sp( coeff_weights, p1 ) - max_offset_weights;
}

DTP Str UTP::type_name() {
    return "PointTree_AABB";
}

#undef DTP
#undef UTP
