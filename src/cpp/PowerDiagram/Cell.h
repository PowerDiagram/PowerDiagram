#pragma once

#include "MapOfUniquePISortedArray.h"
#include "PowerDiagram/PrevCutInfo.h"
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
template<class Config>
class Cell { STD_TL_TYPE_INFO( Cell, "", nb_active_vertices, vertex_indices, vertices, cuts ) //
public:
    static constexpr int         nb_dims                   = Config::nb_dims;
    using                        TF                        = Config::Scalar;

    using                        Vertex                    = CellVertex<TF,nb_dims>;
    using                        Cut                       = CellCut<Config>;
    using                        Ptr                       = PointTree<Config>;
    using                        Pt                        = Vec<TF,nb_dims>;
    
    /**/                         Cell                      ();

    void                         init_geometry_to_encompass( const Pt &min_pos, const Pt &max_pos );
    void                         init_geometry_from        ( const Cell &that );

    void                         memory_compaction         ();
    void                         cut_boundary              ( const Pt &dir, TF off, PI num_boundary );
    void                         cut_dirac                 ( const Pt &p1, TF w1, PI i1, Ptr *ptr, PI32 num_in_ptr );
 
    TF                           for_each_cut_with_measure ( const std::function<void( const CellCut<Config> &cut, TF measure )> &f ) const;
    bool                         test_each_vertex          ( const std::function<bool( const Vertex &vertex )> &f ) const; ///< return true to stop
    void                         for_each_vertex           ( const std::function<void( const Vertex &vertex )> &f ) const;
    void                         for_each_edge             ( const std::function<void( const Vec<PI32,nb_dims-1> &num_cuts, Span<const Vertex *,2> vertices )> &f ) const;
    void                         for_each_face             ( const std::function<void( const Vec<PI32,nb_dims-2> &num_cuts, Span<const Vertex *> vertices )> &f ) const;
 
    void                         display_vtk               ( VtkOutput &vo, const std::function<Vec<VtkOutput::TF,3>( const Pt &pt )> &coord_change ) const; ///<
    void                         display_vtk               ( VtkOutput &vo ) const; ///<

    PI                           capa_vertices             () const { return vertices.size(); }
    PI                           nb_vertices               () const { return nb_active_vertices; }
    PI                           capa_cuts                 () const { return cuts.size(); }
 
    void                         get_prev_cut_info         ( PrevCutInfo<Config> &pci );

    bool                         contains                  ( const Pt &x ) const;
    TF                           measure                   () const;
    bool                         is_inf                    () const;
    TF                           height                    ( const Pt &point ) const;
    bool                         empty                     () const;

    PI32                         nb_active_vertices;       ///<
    Vec<PI>                      vertex_indices;           ///< vertex_indices[ 0 .. nb_active_vertices ] => active vertices. vertex_indices[ nb_active_vertices... ] => the other ones
    Vec<Vertex>                  vertices;                 ///< mix of active and inactive ones

    Pt                           min_pos;                  ///<
    Pt                           max_pos;                  ///<
    TF                           w0;                       ///<
    Pt                           p0;                       ///<
    SI                           i0;                       ///<

    Vec<Cut>                     cuts;                     ///< some of them may be inactive

private:
    template<int i> class        MapOfNumCuts              { public: MapOfUniquePISortedArray<i,PI32,PI> map; };
    using                        NumCutMap                 = RangeOfClasses<MapOfNumCuts,0,nb_dims>;

    void                         add_measure_rec           ( auto &res, auto &M, const auto &num_cuts, PI32 prev_vertex, PI op_id, Vec<TF> &measure_for_each_cut ) const;
    void                         add_measure_rec           ( auto &res, auto &M, const auto &num_cuts, PI32 prev_vertex, PI op_id ) const;
    PI                           new_cut_oid               ( PI s = 0 ) const;
 
    Pt                           compute_pos               ( const Pt &p0, const Pt &p1, TF s0, TF s1 ) const;
    Pt                           compute_pos               ( Vec<PI,nb_dims> num_cuts ) const;


    PI                           _remove_ext_vertices      ( PI old_nb_vertices ); ///< return new size
    void                         _add_cut_vertices         ( const Pt &dir, TF off, PI32 new_cut );
    bool                         _all_inside               ( const Pt &dir, TF off );
    void                         _cut                      ( CutType type, const Pt &dir, TF off, const Pt &p1, TF w1, PI i1, Ptr *ptr, PI32 num_in_ptr );

    // intermediate data
    mutable NumCutMap            num_cut_map;              ///<
    mutable PI                   num_cut_oid;              ///< curr op id for num_cut_map
    Vec<TF>                      sps;                      ///< scalar products for each vertex
};

#include "Cell.cxx" // IWYU pragma: export
