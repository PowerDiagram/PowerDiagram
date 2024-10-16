#include "RegularGrid.h"

namespace power_diagram {

RegularGrid::PD_NAME( RegularGrid )( const DiracVec &dv, const Periodicity &periodicity ) {
    //
    auto mima = dv.global_min_max();
    min_pos = mima[ 0 ];
    max_pos = mima[ 1 ];

    //
}


} // namespace power_diagram
