#pragma once

#include "MapOfUniquePISortedArray.h"
#include "RangeOfClasses.h"
#include "CellVertex.h"
#include "VtkOutput.h"
#include "CellCut.h"

//#include "Edge.h"
// #include <amgcl/profiler.hpp>
// extern amgcl::profiler<> *prof;

/**
 * @brief
 *
 */
template<class TS,int nb_dims>
class Cell { STD_TL_TYPE_INFO( Cell, "", min_pos, max_pos, nb_active_vertices, vertex_indices, vertices, cuts ) //
public:
    using                        Vertex                    = CellVertex<TS,nb_dims>;
    using                        Point                     = Vec<TS,nb_dims>;
    using                        Cut                       = CellCut<TS,nb_dims>;
    
    /**/                         Cell                      ();

    void                         init_geometry_to_encompass( const Point &min_pos, const Point &max_pos );
    void                         init_geometry_from        ( const Cell &that );

    void                         cut_boundary              ( const Point &dir, TS off, PI num_boundary );
    void                         cut_dirac                 ( const Point &p1, TS w1, PI i1 );
 
    void                         for_each_vertex           ( const std::function<void( const Vertex &vertex )> &f ) const;
    void                         for_each_edge             ( const std::function<void( const Vec<PI32,nb_dims-1> &num_cuts, Span<const Vertex *,2> vertices )> &f ) const;
    void                         for_each_face             ( const std::function<void( const Vec<PI32,nb_dims-2> &num_cuts, Span<const Vertex *> vertices )> &f ) const;
 
    void                         display_vtk               ( VtkOutput &vo, const std::function<Vec<VtkOutput::TF,3>( const Point &pt )> &coord_change ) const; ///<
    void                         display_vtk               ( VtkOutput &vo ) const; ///<
 
    PI                           nb_vertices               () const { return nb_active_vertices; }
 
    bool                         contains                  ( const Point &x ) const;
    TS                           measure                   () const;
    bool                         is_inf                    () const;
    TS                           height                    ( const Point &point ) const;
    bool                         empty                     () const;

    Point                        min_pos;                  ///<
    Point                        max_pos;                  ///<
    TS                           w0;                       ///<
    Point                        p0;                       ///<
    SI                           i0;                       ///<


private:
    template<int i> class        MapOfNumCuts              { public: MapOfUniquePISortedArray<i,PI32,PI32> map; };
    using                        NumCutMap                 = RangeOfClasses<MapOfNumCuts,0,nb_dims>;

    void                         add_measure_rec           ( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex, const auto &positions, PI op_id ) const;
    void                         add_measure_rec           ( auto &res, auto &M, auto &item_to_vertex, const auto &num_cuts, PI last_vertex, PI op_id ) const;
 
    static void                  apply_corr                ( Vec<int> &keep, auto &v0 );
 
    Point                        compute_pos               ( const Point &p0, const Point &p1, TS s0, TS s1 ) const;
    Point                        compute_pos               ( Vec<PI,nb_dims> num_cuts ) const;

    PI                           _remove_ext_vertices      ( PI old_nb_vertices ); ///< return new size
    void                         _add_cut_vertices         ( PI32 new_cut );
    bool                         _all_inside               ( const Point &dir, TS off );
    void                         _cut                      ( CutType type, const Point &dir, TS off, const Point &p1, TS w1, PI i1 );

    PI32                         nb_active_vertices;       ///<
    Vec<PI32>                    vertex_indices;           ///< vertex_indices[ 0 .. nb_active_vertices ] => active vertices. vertex_indices[ nb_active_vertices... ] => the other ones
    Vec<Vertex>                  vertices;                 ///< mix of active and inactive ones
    Vec<Cut>                     cuts;                     ///< some of them may be inactive

    // intermediate data
    mutable NumCutMap            num_cut_map;              ///<
    mutable PI32                 num_cut_oid;              ///< curr op id for num_cut_map
    Vec<TS>                      sps;                      ///< scalar products for each vertex
};

#include "Cell.cxx" // IWYU pragma: export
