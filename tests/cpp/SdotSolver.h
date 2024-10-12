#pragma once

#include "../../src/cpp/PowerDiagram/VtkOutput.h"
#include "CsrMatrix.h"

template<class Config> class Cell;

/**

*/
class SdotSolver {
public:   
    struct     Config        { using Scalar = double; enum { nb_dims = 3, use_AABB_bounds_on_cells = 0 }; };
         
    using      Pt            = Vec<Config::Scalar,Config::nb_dims>;
    using      TF            = Config::Scalar;
    using      TM            = CsrMatrix<TF>;

    struct     Ders          { Vec<Vec<TF>> M; Vec<TF> V, max_X; TF S; void clear( PI nb_vars ); Vec<TF> argmin() const; };
         
    /**/       SdotSolver    () {}
     
    void       for_each_cell ( const std::function<void( const Cell<Config> &cell, int )> &f, Span<TF> weights );
    void       display_vtk   ( VtkOutput &vo, Span<TF> weights );
   
    TF         target_measure( PI num_cell ) const { return TF( 1 ) / positions.size(); }        
    Vec<TF>    jacobi_dir    ( Span<TF> weights );
    PI         nb_cells      () const;
    Vec<TF>    measures      ( Span<TF> weights );
    Vec<TF>    smooth        ( Span<TF> weights, Span<TF> dir, PI rec );
    TF         error         ( Span<TF> weights, bool *void_cell = nullptr );

    TF         max_alpha_for ( Span<TF> weights, Span<TF> dir );
    Ders       der_err_ap    ( Span<TF> weights, Span<TF> dir, TF eps = 1e-6 );
    Ders       der_err       ( Span<TF> weights, Span<Vec<TF>> dirs );

    auto       matrix_ap     ( Span<TF> weights, TF eps = 1e-6 ) -> Vec<Vec<TF>>;
    TM         matrix        ( Span<TF> weights );
   
    Vec<Pt>    positions;
    Vec<PI>    indices;
   
    Vec<TF>    bnd_offs;
    Vec<Pt>    bnd_dirs;
};
