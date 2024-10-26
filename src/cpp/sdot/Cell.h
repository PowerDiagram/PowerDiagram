#pragma once

#include <tl/support/containers/Opt.h>

#include "MapOfUniquePISortedArray.h"
#include "RangeOfClasses.h"
#include "PrevCutInfo.h"
#include "SimdTensor.h"
#include "VertexRefs.h"
#include "VtkOutput.h"
#include "Cut.h"

namespace sdot {
PD_CLASS_DECL_AND_USE( PavingItem );
PD_CLASS_DECL_AND_USE( Cell );

/**
 * @brief
 *
 */
class PD_NAME( Cell ) { STD_TL_TYPE_INFO( Cell, "", vertex_coords, vertex_refs, cuts )
public:
    using                        VertexCoords              = SimdTensor<TF,nb_dims>;
    
    /**/                         PD_NAME( Cell )           ( const Cell &that );
    /**/                         PD_NAME( Cell )           ();

    void                         init_from                 ( const Cell &that, const Pt &p0, TF w0, PI i0 );

    void                         cut_boundary              ( const Pt &dir, TF off, PI num_boundary );
    void                         cut_dirac                 ( const Pt &p1, TF w1, PI i1, PavingItem *paving_item = nullptr, PI32 num_in_paving_item = 0 );

    void                         remove_inactive_cuts      ();
    void                         memory_compaction         ();
 
    void                         for_each_edge             ( const std::function<void( const Vec<PI32,nb_dims-1> &num_cuts, Span<PI32,2> vertices )> &f ) const;
    void                         for_each_face             ( const std::function<void( const Vec<PI32,nb_dims-2> &num_cuts, Span<PI32> vertices )> &f ) const;
 
    void                         display_vtk               ( VtkOutput &vo, const std::function<Vec<VtkOutput::TF,3>( const Pt &pt )> &to_vtk ) const; ///<
    void                         display_vtk               ( VtkOutput &vo ) const; ///<

    PI                           nb_vertices               () const { return vertex_refs.size(); }
    PI                           nb_cuts                   () const { return cuts.size(); }

    bool                         bounded                   () const { return _bounded; }
 
    void                         get_prev_cut_info         ( PrevCutInfo &pci );

    // areas/...
    TF                           for_each_cut_with_measure ( const std::function<void( const Cut &cut, TF measure )> &f ) const;
    TF                           measure                   () const;

    bool                         contains                  ( const Pt &x ) const;
    bool                         is_inf                    () const;
    TF                           height                    ( const Pt &point ) const;
    bool                         empty                     () const;

    VertexCoords                 vertex_coords;
    Vec<VertexRefs>              vertex_refs;
    Vec<Cut>                     cuts;                     ///< some of them may be inactive

    #if SDOT_CONFIG_AABB_BOUNDS_ON_CELLS
    Pt                           min_pos;                  ///<
    Pt                           max_pos;                  ///<
    #endif
    
    Pt                           p0;                       ///<
    TF                           w0;                       ///<
    PI                           i0;                       ///<

private:
    template<int i> class        MapOfNumCuts              { public: MapOfUniquePISortedArray<i,PI32,PI> map; };
    using                        NumCutMap                 = RangeOfClasses<MapOfNumCuts,0,nb_dims>;

    void                         add_measure_rec           ( auto &res, auto &M, const auto &num_cuts, PI32 prev_vertex, PI op_id, Vec<TF> &measure_for_each_cut ) const;
    void                         add_measure_rec           ( auto &res, auto &M, const auto &num_cuts, PI32 prev_vertex, PI op_id ) const;
    PI                           new_cut_oid               ( PI s = 0 ) const;
 
    Pt                           compute_pos               ( const Pt &p0, const Pt &p1, TF s0, TF s1 ) const;
    Opt<Pt>                      compute_pos               ( Vec<PI,nb_dims> num_cuts ) const;

    void                         _remove_ext_vertices      ( PI old_nb_vertices ); ///< return new size
    void                         _add_cut_vertices         ( const Pt &dir, TF off, PI32 new_cut );
    bool                         _all_inside               ( const Pt &dir, TF off );
    void                         _get_sps                  ( const Pt &dir, TF off );
    void                         _cut                      ( CutType type, const Pt &dir, TF off, const Pt &p1, TF w1, PI i1, PavingItem *ptr, PI32 num_in_ptr );

    bool                         _became_bounded           ();
    void                         _cut_unbounded            ( CutType type, const Pt &dir, TF off, const Pt &p1, TF w1, PI i1, PavingItem *ptr, PI32 num_in_ptr );

    // intermediate data
    mutable NumCutMap            num_cut_map;              ///<
    mutable PI                   num_cut_oid;              ///< curr op id for num_cut_map
    bool                         _bounded;                 ///<
    Vec<TF>                      sps;                      ///< scalar products for each vertex
};

} // namespace sdot
