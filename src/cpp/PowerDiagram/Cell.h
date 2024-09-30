#pragma once

#include "MapOfUniquePISortedArray.h"
#include "CountOfCutTypes.h"
#include "VtkOutput.h"
#include "Vertex.h"
#include "Edge.h"
#include "Cut.h"

#include <amgcl/profiler.hpp>
#include <xsimd/xsimd.hpp>

extern amgcl::profiler<> *prof;

/**
 * @brief
 *
 */
template<class Scalar,int nb_dims>
class Cell { STD_TL_TYPE_INFO( Cell, "", vertices, edges, cuts )
public:
    using                       AlignedVec         = std::vector<Scalar,xsimd::default_allocator<Scalar>>;
    using                       SimdVec            = xsimd::batch<Scalar>;
    using                       Point              = Vec<Scalar,nb_dims>;

    static constexpr PI         simd_size          = SimdVec::size;

    /**/                        Cell               ();

    void                        init_geometry_from ( const Cell &that );
    void                        make_init_simplex  ( const Point &min_pos, const Point &max_pos );
    void                        cut_boundary       ( const Point &dir, Scalar off, PI num_boundary );
    void                        cut_dirac          ( const Point &p1, Scalar w1, PI i1 );

    void                        display_vtk        ( VtkOutput &vo, const std::function<void( Vec<Scalar,3> &pt )> &coord_change ) const; ///<
    void                        display_vtk        ( VtkOutput &vo ) const; ///<

    void                        for_each_vertex    ( const std::function<void( const Vertex<Scalar,nb_dims> &v )> &f ) const;
    void                        for_each_edge      ( const std::function<void( Vec<PI,nb_dims-1> num_cuts, const Vertex<Scalar,nb_dims> &v0, const Vertex<Scalar,nb_dims> &v1 )> &f ) const;
    void                        for_each_face      ( const std::function<void( Vec<PI,nb_dims-2> num_cuts, Span<const Vertex<Scalar,nb_dims> *> vertices )> &f ) const;

    void                        add_cut_types      ( CountOfCutTypes &cct, const auto &num_cuts, SI nb_bnds ) const;
    void                        get_used_fbs       ( Vec<bool> &used_fs, Vec<bool> &used_bs, PI nb_bnds ) const;
    bool                        has_inf_cut        ( const Vertex<Scalar,nb_dims> &vertex ) const;
    bool                        contains           ( const Point &x ) const;
    auto                        measure            ( const auto &get_w ) const;
    Scalar                      measure            () const;
    bool                        is_inf             () const;
    Scalar                      height             ( const Point &point ) const;
    bool                        empty              () const;

    // PI                          vertex_list_size;  ///<
    // PI                          vertex_list_capa;  ///<
    // AlignedVec                  vertex_coords;     ///< x0 x1 x2 x3 y0 y1 y2 y3 x4 x5 ...
    // Vec<Vec<int,nb_dims>>       vertex_cuts;       ///<

    Vec<Vertex<Scalar,nb_dims>> vertices;          ///<
    Vec<Edge<Scalar,nb_dims>>   edges;             ///<
    Vec<Cut<Scalar,nb_dims>>    cuts;              ///<

    Scalar                      w0;                ///<
    Point                       p0;                ///<
    SI                          i0;                ///<

private:
    using                       FaceToInt          = MapOfUniquePISortedArray<PI,nb_dims-2,nb_dims-2,int>;

    T_Ti static auto            array_without_index( const Vec<T,i> &values, PI index );
    T_Ti static auto            array_with_value   ( const Vec<T,i> &a, T value );
    void                        add_measure_rec    ( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex, const auto &positions ) const;
    void                        add_measure_rec    ( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex ) const;
    T_T static void             apply_corr         ( Vec<T> &vec, Vec<int> &keep );
    static bool                 is_ext             ( const Point &pos, const Point &dir, Scalar off );

    bool                        vertex_has_cut     ( const Vertex<Scalar,nb_dims> &vertex, const std::function<bool( SI point_index )> &outside_cut ) const;
    Point                       compute_pos        ( const Point &p0, const Point &p1, Scalar s0, Scalar s1 ) const;
    auto                        compute_pos        ( Vec<PI,nb_dims> num_cuts, const auto &get_w ) const;
    Point                       compute_pos        ( Vec<PI,nb_dims> num_cuts ) const;
    void                        _cut               ( Cut<Scalar,nb_dims>::Type type, const Point &dir, Scalar off, const Point &p1, Scalar w1, SI i1 );

    FaceToInt                   waiting_vertices;  ///<
    Vec<int>                    vertex_corr;       ///<
    Vec<int>                    edge_corr;         ///<
    Vec<Scalar>                 sps;               ///< scalar products for each new cut

    mutable PI                  curr_op_id = 0;    ///<
};

#include "Cell.cxx" // IWYU pragma: export
