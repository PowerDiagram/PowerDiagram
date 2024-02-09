#pragma once

#include "BoxForWeightedPointSet_AABB.h"
#include "../WeightedPointSet.h"
#include <span>

///
template<class Scalar_,class Weight_,int nb_dims_>
class WeightedPointSet_AABB {
public:
    static constexpr int nb_dims               = nb_dims_;
    using                Scalar                = Scalar_;
    using                Weight                = Weight_;
    using                Point                 = Vec<Scalar,nb_dims>;

    using                Box                   = BoxForWeightedPointSet_AABB<Scalar,Point,Weight,nb_dims>;

    static void          for_each_template_arg ( auto &&f );
    static void          get_compilation_flags ( CompilationFlags &cn );
    static auto          template_type_name    ();

    void                 set_points_and_weights( const auto &points, const auto &weights );
    Box*                 make_box_rec          ( const auto &points, const auto &weights, std::span<PI> indices, Box *parent );
    auto*                display               ( Displayer &ds ) const;
    PI                   size                  () const { return nb_points; }

    // params
    bool                 get_axes_using_eigen_values = true;
    PI                   max_nb_points_per_cell = 30;

    Scalar               init_radius_mul = 10;
    PI                   nb_points = 0;
    BumpPointerPool      pool;

    Vec<Box*>            leaves;
    Box*                 root;
};

WeightedPointSet make_WeightedPointSet_AABB_impl( const auto &points, const auto &weights ) {
    using ScalarType = decltype( exact_div( points[ 0 ][ 0 ] * weights[ 0 ], 1 + points[ 0 ][ 1 ] ) );
    constexpr int dim = ct_sizes_of( CT_DECAYED_TYPE_OF( points[ 0 ] ) ).template get<0>();

    WeightedPointSet_AABB<ScalarType,DECAYED_TYPE_OF( weights[ 0 ] ),dim> res;
    res.set_points_and_weights( points, weights );
    return { FromValue(), std::move( res ) };
}

#include "WeightedPointSet_AABB.tcc" // IWYU pragma: export
