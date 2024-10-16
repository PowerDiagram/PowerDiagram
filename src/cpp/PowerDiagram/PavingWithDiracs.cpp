#include "PavingWithDiracs.h"
#include "RegularGrid.h"

namespace power_diagram {

UniquePtr<PavingWithDiracs> PavingWithDiracs::New( const DiracVec &dv, const CreationParameters &cp ) {
    return UniquePtr<PavingWithDiracs>{ new RegularGrid( dv, RegularGrid::Periodicity{} ) };
}

} // namespace power_diagram
