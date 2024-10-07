#pragma once

#include "MapOfUniquePISortedArray.h"
#include "RangeOfClasses.h"
#include "CellVertex.h"
#include "VtkOutput.h"
#include "CellCut.h"

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
    using                        Cut                       = CellCut<TS,nb_dims>;
    using                        Pt                        = Vec<TS,nb_dims>;
    
    /**/                         Cell                      ();

    void                         init_geometry_to_encompass( const Pt &min_pos, const Pt &max_pos );
    void                         init_geometry_from        ( const Cell &that );

    void                         memory_compaction         ();
    void                         cut_boundary              ( const Pt &dir, TS off, PI num_boundary );
    void                         cut_dirac                 ( const Pt &p1, TS w1, PI i1 );
 
    bool                         test_each_vertex          ( const std::function<bool( const Vertex &vertex )> &f ) const; ///< return true to stop
    void                         for_each_vertex           ( const std::function<void( const Vertex &vertex )> &f ) const;
    void                         for_each_edge             ( const std::function<void( const Vec<PI32,nb_dims-1> &num_cuts, Span<const Vertex *,2> vertices )> &f ) const;
    void                         for_each_face             ( const std::function<void( const Vec<PI32,nb_dims-2> &num_cuts, Span<const Vertex *> vertices )> &f ) const;
 
    void                         display_vtk               ( VtkOutput &vo, const std::function<Vec<VtkOutput::TF,3>( const Pt &pt )> &coord_change ) const; ///<
    void                         display_vtk               ( VtkOutput &vo ) const; ///<
 
    PI                           capa_vertices             () const { return vertices.size(); }
    PI                           nb_vertices               () const { return nb_active_vertices; }
    PI                           capa_cuts                 () const { return cuts.size(); }
 
    bool                         contains                  ( const Pt &x ) const;
    TS                           measure                   () const;
    bool                         is_inf                    () const;
    TS                           height                    ( const Pt &point ) const;
    bool                         empty                     () const;

    Pt                           min_pos;                  ///<
    Pt                           max_pos;                  ///<
    TS                           w0;                       ///<
    Pt                           p0;                       ///<
    SI                           i0;                       ///<

    Vec<Cut>                     cuts;                     ///< some of them may be inactive

private:
    template<int i> class        MapOfNumCuts              { public: MapOfUniquePISortedArray<i,PI32,PI> map; };
    using                        NumCutMap                 = RangeOfClasses<MapOfNumCuts,0,nb_dims>;

    void                         add_measure_rec           ( auto &res, auto &M, const auto &num_cuts, PI32 prev_vertex, PI op_id ) const;
 
    static void                  apply_corr                ( Vec<int> &keep, auto &v0 );
 
    Pt                           compute_pos               ( const Pt &p0, const Pt &p1, TS s0, TS s1 ) const;
    Pt                           compute_pos               ( Vec<PI,nb_dims> num_cuts ) const;

    PI                           _remove_ext_vertices      ( PI old_nb_vertices ); ///< return new size
    void                         _add_cut_vertices         ( const Pt &dir, TS off, PI32 new_cut );
    bool                         _all_inside               ( const Pt &dir, TS off );
    void                         _cut                      ( CutType type, const Pt &dir, TS off, const Pt &p1, TS w1, PI i1 );

    PI32                         nb_active_vertices;       ///<
    Vec<PI32>                    vertex_indices;           ///< vertex_indices[ 0 .. nb_active_vertices ] => active vertices. vertex_indices[ nb_active_vertices... ] => the other ones
    Vec<Vertex>                  vertices;                 ///< mix of active and inactive ones

    // intermediate data
    mutable NumCutMap            num_cut_map;              ///<
    mutable PI                   num_cut_oid;              ///< curr op id for num_cut_map
    Vec<TS>                      sps;                      ///< scalar products for each vertex

    int                          cut_count = 0;
};

#include "Cell.cxx" // IWYU pragma: export
