#pragma once

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
class InfCell { STD_METIL_TYPE_INFO( InfCell, "", vertices, cuts )
public:
    using                       Point              = Vec<Scalar,nb_dims>;

    void                        cut                ( const Point &dir, Scalar off, SI point_index );

    void                        display_vtk        ( VtkOutput &vo, const std::function<void( VtkOutput::Pt &pt )> &coord_change ) const; ///<
    void                        display_vtk        ( VtkOutput &vo ) const; ///<

    void                        for_each_vertex    ( const std::function<void( const Vertex<Scalar,nb_dims> &v )> &f ) const;
    Scalar                      height             ( const Point &point ) const;

    const Scalar*               orig_weight;       ///<
    const Point*                orig_point;        ///<
    SI                          orig_index;        ///<

    Vec<Vertex<Scalar,nb_dims>> vertices;          ///<
    Vec<Cut<Scalar,nb_dims>>    cuts;              ///<

private:
    TTi static auto             array_without_index( const Vec<T,i> &values, PI index );
    TTi static auto             array_with_value   ( const Vec<T,i> &a, T value );
    TT static void              apply_corr         ( Vec<T> &vec, Vec<int> &keep );

    void                        clean_inactive_cuts();
    Point                       compute_pos        ( Vec<PI,nb_dims> num_cuts ) const;

    // Vec<int>                 vertex_corr;       ///<
    // Vec<Scalar>              sps;               ///<

    // mutable PI               curr_op_id = 0;    ///<
};

#include "InfCell.tcc"
