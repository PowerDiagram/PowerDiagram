#pragma once

#include <tl/support/operators/argmax.h>
#include <tl/support/operators/argmin.h>
#include <tl/support/TODO.h>
#include <tl/support/P.h>

#include "PointTree_AABB.h"

#include <eigen3/Eigen/LU>

#define DTP template<class Scalar,int nb_dims>
#define UTP PointTree_AABB<Scalar,nb_dims>

DTP UTP::PointTree_AABB( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights, Span<PI> indices, PointTree<Scalar,nb_dims> *parent, PI num_in_parent ) : PointTreeWithValues<Scalar,nb_dims>( points, weights, indices, parent, num_in_parent ) {
    num_sym = 0;
    
    init_bounds( cp );
    init_children( cp );
}

#ifndef AVOID_DISPLAY
DTP void UTP::display( Displayer &ds ) const {
    ds << "murge";
    // const auto &children = this->children;
    // if ( children.size() )
    //     return DS_OBJECT( PointTree_AABB, min_pos, max_pos, children );

    // const auto &weights = this->weights;
    // const auto &points = this->points;
    // return DS_OBJECT( PointTree_AABB, min_pos, max_pos, points, weights );
}
#endif

DTP void UTP::init_children( const PointTreeCtorParms &cp ) {
    using namespace std;

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

    if ( cp.divide_by_nd ) { // create 2^d children
        Point sep = ( max_pos + min_pos ) / 2;

        const auto ind = [&]( const Point &point ) {
            PI res = 0;
            for( PI d = 0, o = 1; d < nb_dims; ++d, o *= 2 )
                res += o * ( point[ d ] > sep[ d ] );
            return res;
        };

        constexpr PI ns = ( 1 << nb_dims );
        Vec<PI,ns + 1> nb_points_per_sub_box( FromItemValue(), 0 );
        for( PI i = 0; i < n; ++i )
            ++nb_points_per_sub_box[ ind( this->points[ i ] ) ];

        for( PI s = 0, a = 0; s <= ns; ++s )
            nb_points_per_sub_box[ s ] = exchange( a, a + nb_points_per_sub_box[ s ] );

        const Vec<PI,ns + 1> ends = nb_points_per_sub_box;
        for( PI s = 0; s < ns; ++s ) {
            for( PI i = nb_points_per_sub_box[ s ]; i < ends[ s + 1 ]; ) {
                PI t = ind( this->points[ i ] );
                if ( t == s ) {
                    ++nb_points_per_sub_box[ s ];
                    ++i;
                } else {
                    PI j = nb_points_per_sub_box[ t ];
                    swap( this->indices[ i ], this->indices[ j ] );
                    swap( this->weights[ i ], this->weights[ j ] );
                    swap( this->points [ i ], this->points [ j ] );

                    ++nb_points_per_sub_box[ t ];
                }
            }
        }

        for( PI s = 0; s < ns; ++s ) {
            this->children << new PointTree_AABB( cp,
                this->points .subspan( ends[ s + 0 ], ends[ s + 1 ] ),
                this->weights.subspan( ends[ s + 0 ], ends[ s + 1 ] ),
                this->indices.subspan( ends[ s + 0 ], ends[ s + 1 ] ),
                this,
                s
            );
        }
    } else { // create 2 children
        PI dd = argmax( max_pos - min_pos );
        Scalar sep = ( max_pos[ dd ] + min_pos[ dd ] ) / 2;

        if ( cp.make_histogram ) {
            constexpr PI histo_size = 60;
            Vec<PI,histo_size + 1> histo( FromItemValue(), 0 );
            Scalar coeff = histo_size / ( max_pos[ dd ] - min_pos[ dd ] ) * ( 1 - 10 * numeric_limits<Scalar>::epsilon() );
            for( PI i = 0; i < n; ++i )
                ++histo[ int( ( this->points[ i ][ dd ] - min_pos[ dd ] ) * coeff ) ];
            // P( n, histo );

            for( PI s = 0, a = 0; s <= histo_size; ++s )
                histo[ s ] = abs( exchange( a, a + histo[ s ] ) - n / 2.0 );
            // P( histo );

            PI num = max( 2, min( histo_size - 3, argmin( histo ) ) );
            sep = min_pos[ dd ] + num * ( max_pos[ dd ] - min_pos[ dd ] ) / histo_size;
        }

        // inplace swap
        PI i = 0;
        for( PI j = n - 1; ; ) {
            // while we have points at the right place
            while ( this->points[ i ][ dd ] <= sep )
                ++i;
            while ( this->points[ j ][ dd ] > sep )
                --j;
            if ( j < i )
                break;

            // swap the 2 points that are at the wrong places
            swap( this->indices[ i ], this->indices[ j ] );
            swap( this->weights[ i ], this->weights[ j ] );
            swap( this->points [ i ], this->points [ j ] );
            ++i;
            --j;
            if ( j < i )
                break;
        }

        this->children << new PointTree_AABB( cp, this->points.subspan( 0, i ), this->weights.subspan( 0, i ), this->indices.subspan( 0, i ), this, 0 );
        this->children << new PointTree_AABB( cp, this->points.subspan( i, n ), this->weights.subspan( i, n ), this->indices.subspan( i, n ), this, 1 );
    }
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

// DTP bool UTP::may_intersect( const SimdTensor<Scalar,nb_dims> &vertices, const Point &p0, Scalar w0 ) const {
//     using namespace xsimd;
//     using namespace std;

//     using SimdVec = SimdTensor<Scalar,nb_dims>::SimdVec;
//     constexpr PI simd_size = SimdVec::size;

//     //return norm_2_p2( vertex - p0 ) > norm_2_p2( vertex - p1 ) - sp( coeff_weights, p1 ) - max_offset_weights - w0;
//     // Point p1 = min( max_pos, max( min_pos, vertex + Scalar( 1 ) / 2 * coeff_weights ) );
//     const PI floor_of_nb_vertices = vertices.size() / simd_size * simd_size;
//     for( PI n = 0; n < floor_of_nb_vertices; n += simd_size ) {
//         // return norm_2_p2( vertex - p0 ) - norm_2_p2( vertex - p1 ) + sp( coeff_weights, p1 ) + max_offset_weights - w0 > 0;        
//         SimdVec res = max_offset_weights - w0;
//         for( int d = 0; d < nb_dims; ++d ) {
//             SimdVec pos = SimdVec::load_aligned( vertices.data() + vertices.offset( n, d ) );
      
//             SimdVec p1d = xsimd::min( SimdVec( max_pos[ d ] ), xsimd::max( SimdVec( min_pos[ d ] ), pos + Scalar( 1 ) / 2 * coeff_weights[ d ] ) );
//             SimdVec vp0 = pos - p0[ d ];
//             SimdVec vp1 = pos - p1d;

//             res += vp0 * vp0;
//             res -= vp1 * vp1;
//             res -= coeff_weights[ d ] * p1d;
//         }

//         if ( xsimd::any( res > 0 ) )
//             return true;
//     }

//     for( PI n = floor_of_nb_vertices; n < vertices.size(); ++n ) {
//         // Point p1 = inv_sym( q1, this->num_sym );
//         Point vp = vertices[ n ];
//         Point p1 = min( max_pos, max( min_pos, vp + Scalar( 1 ) / 2 * coeff_weights ) );
//         if ( norm_2_p2( vp - p0 ) - w0 > norm_2_p2( vp - p1 ) - sp( coeff_weights, p1 ) - max_offset_weights )
//             return true;
//     }

//     return false;
// }

DTP bool UTP::may_intersect( const Cell<Scalar,nb_dims> &cell ) const {
    // o = norm_2_p2( pos_in_cell - cell.p0 ) - cell.w0 - norm_2_p2( pos_in_cell - pos_in_box ) + sp( coeff_weights, pos_in_box ) + max_offset_weights
    // o = norm_2_p2( cell.p0 ) + 2 * sp( pos_in_cell, pos_in_box - cell.p0 ) - cell.w0 - norm_2_p2( pos_in_box ) + sp( coeff_weights, pos_in_box ) + max_offset_weights
    //  -> 

    return cell.test_each_vertex( [&]( const CellVertex<Scalar,nb_dims> &vertex ) -> bool {
        Point p1 = min( max_pos, max( min_pos, vertex.pos + Scalar( 1 ) / 2 * coeff_weights ) );
        return norm_2_p2( vertex.pos - cell.p0 ) - cell.w0 > norm_2_p2( vertex.pos - p1 ) - sp( coeff_weights, p1 ) - max_offset_weights;
    } );
}

DTP Str UTP::type_name() {
    return "PointTree_AABB";
}

#undef DTP
#undef UTP
