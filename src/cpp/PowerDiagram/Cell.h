#pragma once

#include "MapOfUniquePISortedArray.h"
#include "CountOfCutTypes.h"
#include "VtkOutput.h"
#include "Vertex.h"
#include "Edge.h"
#include "Cut.h"

/**
 * @brief 
 * 
 */
template<class Scalar,int nb_dims>
class Cell { STD_METIL_TYPE_INFO( Cell, "", vertices, edges, cuts )
public:
    using                       Point              = Vec<Scalar,nb_dims>;

    void                        init_geometry_from ( const Cell &that );
    void                        make_init_simplex  ( const Point &min_pos, const Point &max_pos );
    void                        cut                ( const Point &dir, Scalar off, SI point_index );

    void                        display_vtk        ( VtkOutput &vo, const std::function<void( VtkOutput::Pt &pt )> &coord_change ) const; ///<
    void                        display_vtk        ( VtkOutput &vo ) const; ///<

    void                        for_each_vertex    ( const std::function<void( const Vertex<Scalar,nb_dims> &v )> &f ) const;
    void                        for_each_edge      ( const std::function<void( Vec<PI,nb_dims-1> num_cuts, const Vertex<Scalar,nb_dims> &v0, const Vertex<Scalar,nb_dims> &v1 )> &f ) const;
    void                        for_each_face      ( const std::function<void( Vec<PI,nb_dims-2> num_cuts, Span<const Vertex<Scalar,nb_dims> *> vertices )> &f ) const;

    void                        add_cut_types      ( CountOfCutTypes &cct, const Vertex<Scalar,nb_dims> &vertex, SI nb_bnds ) const;
    void                        get_used_fbs       ( Vec<bool> &used_fs, Vec<bool> &used_bs, PI nb_bnds ) const;
    bool                        has_inf_cut        ( const Vertex<Scalar,nb_dims> &vertex ) const;
    bool                        contains           ( const Point &x ) const;
    bool                        is_inf             () const;
    Scalar                      height             ( const Point &point ) const;
    bool                        empty              () const;

    const Scalar*               orig_weight;       ///<
    const Point*                orig_point;        ///<
    SI                          orig_index;        ///<

    Vec<Vertex<Scalar,nb_dims>> vertices;          ///<
    Vec<Edge<Scalar,nb_dims>>   edges;             ///<
    Vec<Cut<Scalar,nb_dims>>    cuts;              ///<

private:
    using                       FaceToInt          = MapOfUniquePISortedArray<PI,nb_dims-2,int>;

    TTi static auto             array_without_index( const Vec<T,i> &values, PI index );
    TTi static auto             array_with_value   ( const Vec<T,i> &a, T value );
    TT static void              apply_corr         ( Vec<T> &vec, Vec<int> &keep );
    static bool                 is_ext             ( const Point &pos, const Point &dir, Scalar off );

    bool                        vertex_has_cut     ( const Vertex<Scalar,nb_dims> &vertex, const std::function<bool( SI point_index )> &outside_cut ) const;
    Point                       compute_pos        ( const Point &p0, const Point &p1, Scalar s0, Scalar s1 ) const;
    Point                       compute_pos        ( Vec<PI,nb_dims> num_cuts ) const;

    FaceToInt                   waiting_vertices;  ///<
    Vec<int>                    vertex_corr;       ///<
    Vec<int>                    edge_corr;         ///<
    Vec<Scalar>                 sps;               ///<

    mutable PI                  curr_op_id = 0;    ///<
};

#include "Cell.tcc"
