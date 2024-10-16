#pragma once

#include "CutType.h"
#include "Config.h"

namespace power_diagram {
PD_CLASS_DECL_AND_USE( PavingItem );
PD_CLASS_DECL_AND_USE( Cut );

/**
 * @brief 
 * 
 */
class PD_NAME( Cut ) { STD_TL_TYPE_INFO( Cut, "", type, dir, off, i1 ) // , p1, w1
public:
    static Cut           boundary_cut  ( const Pt &dir, TF off, PI boundary_index ) { return { .type = CutType::Boundary, .dir = dir, .off = off, .i1 = boundary_index }; }
    static Cut           dirac_cut     ( const Pt &dir, TF off, const Pt &p1, TF w1, PI i1, PavingItem *paving_item, PI num_in_paving_item ) { return { CutType::Dirac, dir, off, num_in_paving_item, paving_item, p1, w1, i1 }; }
    static Cut           inf_cut       ( const Pt &dir, TF off, PI inf_index ) { return { .type = CutType::Infinity, .dir = dir, .off = off, .i1 = inf_index }; }
          
    bool                 is_inf        () const { return type == CutType::Infinity; }

    // common data
    CutType              type;
    Pt                   dir;
    TF                   off;

    // dirac cut data
    PI                   num_in_paving_item;
    PavingItem*          paving_item;
    Pt                   p1;
    TF                   w1;
    PI                   i1;
};

} // namespace power_diagram
