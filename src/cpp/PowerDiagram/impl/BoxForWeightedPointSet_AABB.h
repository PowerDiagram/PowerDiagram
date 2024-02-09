#pragma once

#include <vfs/support/CompilationFlags.h>

/**
*/
template<class Scalar,class Point,class Weight,int nb_dims>
class BoxForWeightedPointSet_AABB {
public:
    using        Box             = BoxForWeightedPointSet_AABB;

    void         for_each_box_rec( const auto &f ) { f( this ); for( Box *c : children ) c->for_each_box_rec( f ); }
    void         for_each_point  ( const auto &f ) const { for( PI i = 0; i < points.size(); ++i ) f( points[ i ], weights[ i ], indices[ i ] ); }
    DisplayItem* display         ( Displayer &ds ) const { return DS_OBJECT( Box, min_pos, max_pos, indices, weights, points, children ); }
    Scalar       radius          () const { return max( max_pos - min_pos ); }
    Point        center          () const { return ( max_pos + min_pos ) / 2; }

    Box*         sibling         = nullptr; ///<
    Box*         parent          = nullptr; ///<

    Vec<Box *>   children;       ///<
    Point        min_pos;        ///<
    Point        max_pos;        ///<
    Vec<PI>      indices;        ///<
    Vec<Weight>  weights;        ///<
    Vec<Point>   points;         ///<
};

