#include "PavingWithDiracs.h"
#include "RegularGrid.h"
#include <thread>

namespace power_diagram {

UniquePtr<PavingWithDiracs> PavingWithDiracs::New( const DiracVec &dv, const CreationParameters &cp ) {
    return UniquePtr<PavingWithDiracs>{ new RegularGrid( dv, RegularGrid::Periodicity{} ) };
}

int PavingWithDiracs::max_nb_threads() const {
    return std::thread::hardware_concurrency();
}

void PavingWithDiracs::spawn( const std::function<void( int num_thread, int nb_threads )> &f ) const {
    // launch
    const PI nb_threads = max_nb_threads();
    Vec<std::thread> threads( FromSizeAndInitFunctionOnIndex(), nb_threads, [&]( std::thread *th, PI num_thread ) {
        new ( th ) std::thread( [num_thread,nb_threads,&f]() {
            f( num_thread, nb_threads );
        } );
    } );

    // join
    for( std::thread &th : threads )
        th.join();
}

} // namespace power_diagram
