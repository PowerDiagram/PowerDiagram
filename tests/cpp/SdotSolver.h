#pragma once

#include "PowerDiagram/PowerDiagram.h"
#include "CsrMatrix.h"

/**

*/
class SdotSolver {
public:   
    struct  Config       { using Scalar = double; enum { nb_dims = 3, use_AABB_bounds_on_cells = 0 }; };
        
    using   Pd           = PowerDiagram<Config>;
    using   TF           = Config::Scalar;
    using   TM           = CsrMatrix<TF>;
    using   Pt           = Pd::Pt;

     
    /**/    SdotSolver   () {}
 
    void    for_each_cell( const std::function<void( const Cell<Config> &cell, int )> &f, Span<TF> weights );
    void    display_vtk  ( VtkOutput &vo, Span<TF> weights );

    Vec<TF> jacobi_dir   ( Span<TF> weights );
    PI      nb_cells     () const;
    Vec<TF> measures     ( Span<TF> weights );
    auto    der_err      ( Span<TF> weights, Span<TF> dir ) -> Vec<TF,3>;
    TF      error        ( Span<TF> weights );

    auto    matrix_ap    ( Span<TF> weights, TF eps = 1e-6 ) -> Vec<Vec<TF>>;
    TM      matrix       ( Span<TF> weights );

    Vec<Pt> positions;
    Vec<PI> indices;

    Vec<TF> bnd_offs;
    Vec<Pt> bnd_dirs;
};
