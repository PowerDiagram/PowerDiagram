#pragma once

#include "MapOfUniquePISortedArray.h"
#include "VtkOutput.h"
#include "Vertex.h"
#include "Edge.h"
#include "Cut.h"

#define Cell CC_DT( Cell )

/**
 * @brief 
 * 
 */
class Cell { STD_METIL_TYPE_INFO( Cell, "", vertices, edges, cuts )
public:
    void            init_geometry_from ( const Cell &that );
    void            make_init_simplex  ( const Point &center, Scalar radius );
    void            cut                ( const Point &dir, Scalar off, SI point_index );

    void            display_vtk        ( VtkOutput &vo, const std::function<void( VtkOutput::Pt &pt )> &coord_change ) const; ///<
    void            display_vtk        ( VtkOutput &vo ) const; ///<

    void            for_each_vertex    ( const std::function<void( const Vertex &v )> &f ) const;
    void            for_each_edge      ( const std::function<void( Vec<PI,PD_DIM-1> num_cuts, const Vertex *v0, const Vertex *v1 )> &f ) const;
    void            for_each_face      ( const std::function<void( Vec<PI,PD_DIM-2> num_cuts, Span<const Vertex *> vertices )> &f ) const;

    const Scalar*   orig_weight;       ///<
    const Point*    orig_point;        ///<
    SI              orig_index;        ///<

    Vec<Vertex>     vertices;          ///<
    Vec<Edge>       edges;             ///<
    Vec<Cut>        cuts;              ///<

private:
    using           FaceToInt          = MapOfUniquePISortedArray<PI,PD_DIM-2,int>;

    TTi static auto array_without_index( const Vec<T,i> &values, PI index );
    TTi static auto array_with_value   ( const Vec<T,i> &a, T value );
    TT static void  apply_corr         ( Vec<T> &vec, Vec<int> &keep );
    static bool     is_ext             ( const Point &pos, const Point &dir, Scalar off );

    bool            vertex_has_cut     ( const Vertex &vertex, const std::function<bool( SI point_index )> &outside_cut ) const;
    Point           compute_pos        ( const Point &p0, const Point &p1, Scalar s0, Scalar s1 ) const;
    Point           compute_pos        ( Vec<PI,PD_DIM> num_cuts ) const;

    FaceToInt       waiting_vertices;  ///<
    Vec<int>        vertex_corr;       ///<
    Vec<int>        edge_corr;         ///<
    Vec<Scalar>     sps;               ///<

    mutable PI      curr_op_id = 0;    ///<
};
