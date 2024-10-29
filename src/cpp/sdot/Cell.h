#pragma once

#include <tl/support/containers/Opt.h>

#include "MapOfUniquePISortedArray.h"
#include "RangeOfClasses.h"
#include "PrevCutInfo.h"
#include "BigRational.h"
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
class PD_NAME( Cell ) { STD_TL_TYPE_INFO( Cell, "", vertex_coords, vertex_refs, _empty, cuts )
public:
    using                        VertexCoords                = SimdTensor<TF,nb_dims>;
    using                        VertexRef                   = Vec<PI32,nb_dims>;

    /**/                         PD_NAME( Cell )             ( const Cell &that );
    /**/                         PD_NAME( Cell )             ();
  
    void                         init_from                   ( const Cell &that, const Pt &p0, TF w0, PI i0 );

    void                         remove_inactive_cuts        ();
    void                         memory_compaction           ();
    void                         cut_boundary                ( const Pt &dir, TF off, PI num_boundary );
    void                         cut_dirac                   ( const Pt &p1, TF w1, PI i1, PavingItem *paving_item = nullptr, PI32 num_in_paving_item = 0 );
   
    void                         get_prev_cut_info           ( PrevCutInfo &pci );
    void                         for_each_edge               ( const std::function<void( const Vec<PI32,nb_dims-1> &num_cuts, Span<PI32,2> vertices )> &f ) const;
    void                         for_each_face               ( const std::function<void( const Vec<PI32,nb_dims-2> &num_cuts, Span<PI32> vertices )> &f ) const;
   
    void                         display_vtk                 ( VtkOutput &vo, const std::function<Vec<VtkOutput::TF,3>( const Pt &pt )> &to_vtk ) const; ///<
    void                         display_vtk                 ( VtkOutput &vo ) const; ///<
  
    // areas/...  
    TF                           for_each_cut_with_measure   ( const std::function<void( const Cut &cut, TF measure )> &f ) const;
    TF                           measure                     () const;

    PI                           nb_vertices                 () const { return vertex_refs.size(); }
    PI                           nb_cuts                     () const { return cuts.size(); }
    bool                         bounded                     () const { return _bounded; }
    bool                         empty                       () const;
  
    bool                         contains                    ( const Pt &point ) const;
    TF                           height                      ( const Pt &point ) const;
  
    VertexCoords                 vertex_coords;              ///< positions of the vertices
    Vec<VertexRef>               vertex_refs;                ///< [num_cut] for each vertex
    Vec<Cut>                     cuts;                       ///< some of them may be inactive
  
    #if SDOT_CONFIG_AABB_BOUNDS_ON_CELLS  
    Pt                           min_pos;                    ///<
    Pt                           max_pos;                    ///<
    #endif  
      
    Pt                           p0;                         ///< dirac pos
    TF                           w0;                         ///< dirac weight
    PI                           i0;                         ///< dirac index
  
private:  
    template<int d> class        NumCutMapForDim             { public: MapOfUniquePISortedArray<d,PI32,PI> map; };
    using                        NumCutMap                   = RangeOfClasses<NumCutMapForDim,0,nb_dims>;
  
    template<int d> struct       LowerDimCut                 { Vec<BigRational,d> dir; BigRational off; void display( Displayer &ds ) const { ds << dir << off; } };

    template<int d> class        LowerDimDataForDim          { public: Vec<Vec<BigRational,nb_dims>,d> base; Vec<Vec<BigRational,d>> vertex_coords; Vec<Vec<PI32,d>> vertex_refs; Vec<LowerDimCut<d>> cuts; };
    using                        LowerDimData                = RangeOfClasses<LowerDimDataForDim,0,nb_dims>;
  
    void                         add_measure_rec             ( auto &res, auto &M, const auto &num_cuts, PI32 prev_vertex, PI op_id, Vec<TF> &measure_for_each_cut ) const;
    void                         add_measure_rec             ( auto &res, auto &M, const auto &num_cuts, PI32 prev_vertex, PI op_id ) const;
    PI                           new_cut_oid                 ( PI s = 0 ) const;
   
    Pt                           compute_pos                 ( const Pt &p0, const Pt &p1, TF s0, TF s1 ) const;
    Opt<Pt>                      compute_pos                 ( const auto &cuts, const auto &num_cuts ) const;
  
    void                         _remove_inactive_cuts       ( auto &vertex_refs, auto &cuts );
    void                         _remove_ext_vertices        ( PI old_nb_vertices ); ///< return new size
    void                         _add_cut_vertices           ( const Pt &dir, TF off, PI32 new_cut, PI old_nb_vertices );
    bool                         _all_inside                 ( const Pt &dir, TF off );
    void                         _get_sps                    ( const Pt &dir, TF off );
    void                         _cut                        ( CutType type, const Pt &dir, TF off, const Pt &p1, TF w1, PI i1, PavingItem *ptr, PI32 num_in_ptr );

    T_i void                     _vertex_phase_unbounded_cuts( CtInt<i> true_nb_dims, const auto &dir, TF off, auto &vertex_coords, auto &vertex_refs, auto &cuts );
    bool                         _became_bounded             ();
    void                         _unbounded_cut              ( CutType type, const Pt &dir, TF off, const Pt &p1, TF w1, PI i1, PavingItem *ptr, PI32 num_in_ptr );
  
    // intermediate data  
    PI                           _true_dimensionnality;      ///< span of cuts[ ... ].dir
    LowerDimData                 _lower_dim_data;            ///< data used when "true" dimensionnality is not yet nb_dims
    mutable NumCutMap            _num_cut_map;               ///<
    mutable PI                   _num_cut_oid;               ///< curr op id for num_cut_map
    bool                         _bounded;                   ///<
    bool                         _empty;                     ///<
    Vec<TF>                      _sps;                       ///< scalar products for each vertex
};

} // namespace sdot
