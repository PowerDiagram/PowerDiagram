#pragma once

#include "MapOfUniquePISortedArray.h"
#include "CountOfCutTypes.h"
#include "SimdTensor.h"
#include "VtkOutput.h"
#include "Cut.h"

//#include "Edge.h"
// #include <amgcl/profiler.hpp>
// extern amgcl::profiler<> *prof;

/**
 * @brief
 *
 */
template<class Scalar,int nb_dims>
class Cell { STD_TL_TYPE_INFO( Cell, "", vertices, edges, cuts )
public:
    using                        VertexCoords          = SimdTensor<Scalar,nb_dims>;  ///<
    using                        VertexCuts            = Vec<Vec<PI,nb_dims>>;        ///<
    using                        Point                 = Vec<Scalar,nb_dims>;
    using                        Otps                  = Vec<std::tuple<Point,Scalar,PI>>;

    /**/                         Cell                  ();
 
    void                         init_geometry_from    ( const Cell &that );
    void                         make_init_simplex     ( const Point &min_pos, const Point &max_pos );
    void                         cut_boundary          ( const Point &dir, Scalar off, PI num_boundary );
    void                         cut_dirac             ( const Point &p1, Scalar w1, PI i1 );
 
    void                         display_vtk           ( VtkOutput &vo, const std::function<void( Vec<Scalar,3> &pt )> &coord_change ) const; ///<
    void                         display_vtk           ( VtkOutput &vo ) const; ///<

    void                         display               ( Displayer &ds ) const; ///<
 
    void                         for_each_vertex       ( const std::function<void( const Point &pos, const Vec<int,nb_dims> &num_cuts )> &f ) const;
    void                         for_each_edge         ( const std::function<void( const Vec<PI,nb_dims-1> &num_cuts, Span<PI,2> vertices )> &f ) const;
    void                         for_each_face         ( const std::function<void( const Vec<PI,nb_dims-2> &num_cuts, Span<PI> vertices )> &f ) const;
 
    PI                           nb_vertices           () const { return vertex_coords.size(); }
 
    void                         add_cut_types         ( CountOfCutTypes &cct, const auto &num_cuts, SI nb_bnds ) const;
    void                         get_used_fbs          ( Vec<bool> &used_fs, Vec<bool> &used_bs, PI nb_bnds ) const;
    bool                         contains              ( const Point &x ) const;
    auto                         measure               ( const auto &get_w ) const;
    Scalar                       measure               () const;
    bool                         is_inf                () const;
    Scalar                       height                ( const Point &point ) const;
    bool                         empty                 () const;

    VertexCoords                 vertex_coords;        ///<
    Vec<Vec<PI,nb_dims>>         vertex_cuts;          ///< sorted num cuts for each vertex
    Vec<Cut<Scalar,nb_dims>>     cuts;                 ///<

    Otps                         otps;                 ///< buffer
    Scalar                       w0;                   ///<
    Point                        p0;                   ///<
    SI                           i0;                   ///<


private:
    using                        AlignedVec            = VertexCoords::AlignedVec;
    using                        EdgeMap               = MapOfUniquePISortedArray<nb_dims-1,nb_dims-1>;

    void                         add_measure_rec       ( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex, const auto &positions, PI op_id ) const;
    void                         add_measure_rec       ( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex, PI op_id ) const;
 
    static void                  apply_corr            ( auto &v0, auto &v1, Vec<int> &keep );
    static void                  apply_corr            ( Vec<int> &keep, auto &v0 );
    static bool                  is_ext                ( const Point &pos, const Point &dir, Scalar off );
 
    void                         reserve_vertices      ( PI nb_vertices );
    void                         resize_vertices       ( PI nb_vertices );
 
    // bool                      vertex_has_cut        ( const Vertex<Scalar,nb_dims> &vertex, const std::function<bool( SI point_index )> &outside_cut ) const;
    Point                        compute_pos           ( const Point &p0, const Point &p1, Scalar s0, Scalar s1 ) const;
    auto                         compute_pos           ( Vec<PI,nb_dims> num_cuts, const auto &get_w ) const;
    Point                        compute_pos           ( Vec<PI,nb_dims> num_cuts ) const;

    PI                           _remove_ext_vertices  ( PI old_nb_vertices ); ///< return new size
    void                         _add_cut_vertices     ( PI new_cut );
    bool                         _all_inside           ( const Point &dir, Scalar off );
    void                         _get_sps              ( const Point &dir, Scalar off );
    void                         _cut                  ( CutType type, const Point &dir, Scalar off, const Point &p1, Scalar w1, PI i1 );

    // int                       last_inactive_cut;    ///<
    // int                       last_active_cut;      ///<
    AlignedVec                   sps;                  ///< scalar products for each new cut

    mutable PI                   curr_op_id;           ///<
    mutable EdgeMap              edge_map;             ///<
};

#include "Cell.cxx" // IWYU pragma: export
