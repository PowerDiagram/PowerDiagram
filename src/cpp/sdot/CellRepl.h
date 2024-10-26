#pragma once

#include "MapOfUniquePISortedArray.h"
#include "CountOfCutTypes.h"
#include "SimdTensor.h"
#include "VtkOutput.h"
#include "Edge.h"
#include "Cut.h"

// #include <amgcl/profiler.hpp>
// extern amgcl::profiler<> *prof;

/**
 * @brief
 *
 */
template<class Scalar,int nb_dims>
class Cell { STD_TL_TYPE_INFO( Cell, "", vertices, edges, cuts )
public: 
    using                        VertexCoords       = SimdTensor<Scalar,nb_dims>;  ///<
    using                        AlignedVec         = VertexCoords::AlignedVec;
    using                        Pt              = Vec<Scalar,nb_dims>;
 
    /**/                         Cell               ();
 
    void                         init_geometry_from ( const Cell &that );
    void                         make_init_simplex  ( const Pt &min_pos, const Pt &max_pos );
    void                         cut_boundary       ( const Pt &dir, Scalar off, PI num_boundary );
    void                         cut_dirac          ( const Pt &p1, Scalar w1, PI i1 );
 
    void                         display_vtk        ( VtkOutput &vo, const std::function<void( Vec<Scalar,3> &pt )> &coord_change ) const; ///<
    void                         display_vtk        ( VtkOutput &vo ) const; ///<
 
    void                         for_each_vertex    ( const std::function<void( const Pt &pos, const Vec<int,nb_dims> &num_cuts )> &f ) const;
    void                         for_each_edge      ( const std::function<void( const Vec<PI,nb_dims-1> &num_cuts, Span<PI,2> vertices )> &f ) const;
    void                         for_each_face      ( const std::function<void( const Vec<PI,nb_dims-2> &num_cuts, Span<PI> vertices )> &f ) const;
 
    PI                           nb_vertices        () const { return vertex_coords.size(); }
 
    void                         add_cut_types      ( CountOfCutTypes &cct, const auto &num_cuts, SI nb_bnds ) const;
    void                         get_used_fbs       ( Vec<bool> &used_fs, Vec<bool> &used_bs, PI nb_bnds ) const;
    bool                         contains           ( const Pt &x ) const;
    auto                         measure            ( const auto &get_w ) const;
    Scalar                       measure            () const;
    bool                         is_inf             () const;
    Scalar                       height             ( const Pt &point ) const;
    bool                         empty              () const;

    VertexCoords                 vertex_coords;     ///<
    Vec<Vec<PI,nb_dims>>         vertex_cuts;       ///<
    Vec<Edge<Scalar,nb_dims>>    edges;             ///<
    Vec<Cut<Scalar,nb_dims>>     cuts;              ///<
 
    Scalar                       w0;                ///<
    Pt                        p0;                ///<
    SI                           i0;                ///<
 
private: 
    using                        FaceToInt          = MapOfUniquePISortedArray<PI,nb_dims-2,nb_dims-2,int>;
 
    T_Ti static auto             array_without_index( const Vec<T,i> &values, PI index );
    T_Ti static auto             array_with_value   ( const Vec<T,i> &a, T value );
    void                         add_measure_rec    ( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex, const auto &positions ) const;
    void                         add_measure_rec    ( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex ) const;
 
    static void                  apply_corr         ( auto &v0, auto &v1, Vec<int> &keep );
    static void                  apply_corr         ( auto &v0, Vec<int> &keep );
    static bool                  is_ext             ( const Pt &pos, const Pt &dir, Scalar off );
 
    void                         reserve_vertices   ( PI nb_vertices );
    void                         resize_vertices    ( PI nb_vertices );
 
    // bool                      vertex_has_cut     ( const Vertex<Scalar,nb_dims> &vertex, const std::function<bool( SI point_index )> &outside_cut ) const;
    Pt                        compute_pos        ( const Pt &p0, const Pt &p1, Scalar s0, Scalar s1 ) const;
    auto                         compute_pos        ( Vec<PI,nb_dims> num_cuts, const auto &get_w ) const;
    Pt                        compute_pos        ( Vec<PI,nb_dims> num_cuts ) const;
    void                         _cut               ( CutType type, const Pt &dir, Scalar off, const Pt &p1, Scalar w1, SI i1 );

    FaceToInt                    waiting_vertices;  ///<
    Vec<int>                     vertex_corr;       ///<
    Vec<int>                     edge_corr;         ///<
    AlignedVec                   sps;               ///< scalar products for each new cut
 
    mutable PI                   curr_op_id = 0;    ///<
};

#include "Cell.cxx" // IWYU pragma: export
