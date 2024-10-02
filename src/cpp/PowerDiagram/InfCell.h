#pragma once

#include <tl/support/containers/Opt.h>
#include "VtkOutput.h"
#include "Vertex.h"
#include "Cut.h"

/**
 * @brief Like a Cell<...> that does not need initial bounds
 *
 * Slower than Cell<...> for cut operations but allows to handle infinite edges
 * 
 */
template<class Scalar,int nb_dims>
class InfCell { STD_TL_TYPE_INFO( InfCell, "", vertices, cuts )
public:
    using                       Point              = Vec<Scalar,nb_dims>;

    void                        cut_boundary       ( const Point &dir, Scalar off, PI num_boundary );
    void                        cut_dirac          ( const Point &p1, Scalar w1, PI i1 );

    void                        display_vtk        ( VtkOutput &vo, const std::function<void( VtkOutput::Pt &pt )> &coord_change ) const; ///<
    void                        display_vtk        ( VtkOutput &vo ) const; ///<

    void                        for_each_repr_point( const std::function<void( const Point &coords )> &f ) const;
    void                        for_each_vertex    ( const std::function<void( const Point &coords, const Vec<PI,nb_dims> &cuts )> &f ) const;
    PI                          nb_vertices        () const { return vertex_coords.size(); }
    Scalar                      height             ( const Point &point ) const;


    Scalar                      w0;                ///<
    Point                       p0;                ///<
    SI                          i0;                ///<

    Vec<Point>                  vertex_coords;     ///<
    Vec<Vec<PI,nb_dims>>        vertex_cuts;       ///<
    Vec<Cut<Scalar,nb_dims>>    cuts;              ///<

    Vec<std::tuple<Point,Scalar,PI>> otps;

private:
    T_Ti static auto            array_without_index( const Vec<T,i> &values, PI index );
    T_Ti static auto            array_with_value   ( const Vec<T,i> &a, T value );
    T_T static void             apply_corr         ( Vec<T> &vec, Vec<int> &keep );

    void                        clean_inactive_cuts();
    bool                        cut_is_useful      ( PI num_cut );
    Opt<Point>                  compute_pos        ( Vec<PI,nb_dims> num_cuts ) const;
    void                        _cut               ( CutType type, const Point &dir, Scalar off, const Point &p1, Scalar w1, PI i1 );
};

#include "InfCell.cxx"
