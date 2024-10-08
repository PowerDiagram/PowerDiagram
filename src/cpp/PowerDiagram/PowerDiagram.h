#pragma once

#include <tl/support/memory/UniquePtr.h>
#include "RemainingBoxes.h"
#include "PointTree.h"
#include "InfCell.h"
#include "Cell.h"

/**
 * @brief 
 * 
 */
template<class TF,int nb_dims>
class PowerDiagram {
public:
    using                   Pt                = Vec<TF,nb_dims>;
    using                   Ptr               = PointTree<TF,nb_dims>;
    using                   CutInfo           = std::map<Ptr *,Vec<PI32>>;

    /**/                    PowerDiagram      ( const PointTreeCtorParms &cp, Vec<Pt> &&points, Vec<TF> &&weights, Span<Pt> bnd_dirs, Span<TF> bnd_offs );

    static Str              type_name         ();

    Pt                      inv_sym           ( const Pt &pt, int num_sym ) const { return pt; }
    Pt                      sym               ( const Pt &pt, int num_sym ) const { return pt; }

    static int              max_nb_threads    ();
    void                    for_each_cell     ( const std::function<void( Cell<TF,nb_dims> &cell, int num_thread )> &f, const CutInfo *prev_cuts = nullptr );
    void                    for_each_cell     ( const std::function<void( Cell<TF,nb_dims> &cell )> &f ); ///< version with a mutex lock for `f`
    auto                    cell_data_at      ( const Pt &pt, TF probe_size ) const -> Vec<std::tuple<const TF *, const Pt *, SI>>;
    auto                    cell_data_at      ( const Pt &pt ) const -> Opt<std::tuple<const TF *, const Pt *, SI>>;
    void                    display_vtk       ( VtkOutput &vo );
    PI                      nb_cells          () const { return point_tree->nb_seed_points(); }

    #ifndef AVOID_DISPLAY
    void                    display           ( Displayer &df ) const;
    #endif

private:
    using                   PtPtr             = UniquePtr<PointTree<TF,nb_dims>>;

    void                    make_intersections( auto &cell, Vec<PI32> &buffer, const RemainingBoxes<TF,nb_dims> &rb_base, const CutInfo *prev_cuts );
    bool                    outside_cell      ( auto &cell, const RemainingBoxes<TF,nb_dims> &rb_base );

    InfCell<TF,nb_dims>     base_inf_cell;    ///<
    Pt                      min_box_pos;      ///<
    Pt                      max_box_pos;      ///<
    PtPtr                   point_tree;       ///<
    Cell<TF,nb_dims>        base_cell;        ///<

    Span<Pt>                bnd_dirs;         ///<
    Span<TF>                bnd_offs;         ///<

    Vec<PI>                 indices;          ///<
    Vec<TF>                 weights;          ///<
    Vec<Pt>                 points;           ///<
};

#include "PowerDiagram.cxx"
