#pragma once

#include <tl/support/memory/UniquePtr.h>
#include "RemainingBoxes.h"
#include "PrevCutInfo.h"
#include "PointTree.h"
#include "Cell.h"

template<class TF,int nd>
struct BasePowerDiagramConfig {
    enum { nb_dims = nd, use_AABB_bounds_on_cells = 0, make_sps_during_phase = 1 }; 
    using Scalar = TF;
};

/**
 * @brief 
 * 
 */
template<class Config>
class PowerDiagram {
public:
    static constexpr int    nb_dims           = Config::nb_dims;
    using                   TF                = Config::Scalar;

    using                   Pt                = Vec<TF,nb_dims>;
    using                   Ptr               = PointTree<Config>;

    /**/                    PowerDiagram      ( const PointTreeCtorParms &cp, Vec<Pt> &&points, Vec<TF> &&weights, Span<Pt> bnd_dirs, Span<TF> bnd_offs );

    static Str              type_name         ();

    Pt                      inv_sym           ( const Pt &pt, int num_sym ) const { return pt; }
    Pt                      sym               ( const Pt &pt, int num_sym ) const { return pt; }

    static int              max_nb_threads    ();
    bool                    for_each_cell     ( const std::function<void( Cell<Config> &cell, int num_thread )> &f, const PrevCutInfo<Config> *prev_cuts = nullptr, bool stop_if_void = true ); ///< return true if stopped
    void                    for_each_cell     ( const std::function<void( Cell<Config> &cell )> &f ); ///< version with a mutex lock for `f`
    auto                    cell_data_at      ( const Pt &pt, TF probe_size ) const -> Vec<std::tuple<const TF *, const Pt *, SI>>;
    auto                    cell_data_at      ( const Pt &pt ) const -> Opt<std::tuple<const TF *, const Pt *, SI>>;
    void                    display_vtk       ( VtkOutput &vo );
    PI                      nb_cells          () const { return point_tree->nb_seed_points(); }

    #ifndef AVOID_DISPLAY
    void                    display           ( Displayer &df ) const;
    #endif

    Cell<Config>            base_cell;        ///<

private:
    using                   PtPtr             = UniquePtr<PointTree<Config>>;

    void                    make_intersections( auto &cell, Vec<PI32> &buffer, RemainingBoxes<Config> &rb, const PrevCutInfo<Config> *prev_cuts );
    bool                    outside_cell      ( auto &cell, const RemainingBoxes<Config> &rb_base );

    Pt                      min_box_pos;      ///<
    Pt                      max_box_pos;      ///<
    PtPtr                   point_tree;       ///<

    Span<Pt>                bnd_dirs;         ///<
    Span<TF>                bnd_offs;         ///<

    Vec<PI>                 indices;          ///<
    Vec<TF>                 weights;          ///<
    Vec<Pt>                 points;           ///<
};

#include "PowerDiagram.cxx"
