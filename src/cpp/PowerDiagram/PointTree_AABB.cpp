#include "PointTree_AABB.h"

PointTree_AABB::PointTree_AABB( const PointTreeCtorParms &cp, Span<Point> points, Span<Scalar> weights ) : weights( weights ), points( points ) {
    init_bounds( cp );
    init_children( cp );
}

DisplayItem *PointTree_AABB::display( DisplayItemFactory &ds ) const {
    if ( children.size() )
        return DS_OBJECT( PointTree_AABB, min_pos, max_pos, children );
    return DS_OBJECT( PointTree_AABB, min_pos, max_pos, points, weights );
}

void PointTree_AABB::init_children( const PointTreeCtorParms &cp ) {
    const PI n = points.size();
    if ( n <= cp.max_nb_points )
        return;

    TODO;
}

void PointTree_AABB::init_bounds( const PointTreeCtorParms &cp ) {
    const PI n = points.size();
    if ( n == 0 )
        return;

    min_offset_weights = weights[ 0 ];
    max_offset_weights = weights[ 0 ];
    coeff_weights = { FromItemValue(), 0 };

    min_pos = points[ 0 ];
    max_pos = points[ 0 ];
    for( PI i = 1; i < n; ++i ) {
        for( PI d = 0; d < PD_DIM; ++d ) {
            min_pos[ d ] = min( min_pos[ d ], points[ i ][ d ] );
            max_pos[ d ] = max( max_pos[ d ], points[ i ][ d ] );
        }

        min_offset_weights = min( min_offset_weights, weights[ i ] );
        max_offset_weights = max( max_offset_weights, weights[ i ] );
    }
}

Str PointTree_AABB::type_name() {
    return "PointTree_AABB";
}
