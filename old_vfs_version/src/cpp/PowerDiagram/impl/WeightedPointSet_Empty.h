#pragma once

#include "../WeightedPointSet.h"

///
class WeightedPointSet_Empty {
public:
    static constexpr int nb_dims               = 0;
    using                Scalar                = double;
    using                Weight                = double;
    using                Point                 = Vec<Scalar,nb_dims>;
    using                Box                   = int;

    static void          get_compilation_flags ( CompilationFlags &cn ) { cn.add_inc_file( "PowerDiagram/impl/WeightedPointSet_Empty.h" ); }
    static auto          type_name             () { return "WeightedPointSet_Empty"; }
    PI                   size                  () const { return 0; }
};

WeightedPointSet make_WeightedPointSet_Empty() {
    return { FromValue(), WeightedPointSet_Empty() };
}
