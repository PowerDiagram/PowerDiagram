#include "PowerDiagram/PavingWithDiracs.h"
#include "catch_main.h"

using namespace power_diagram;

TEST_CASE( "bench Astro 3D", "" ) {
    using PavingWithDiracs = PD_NAME( PavingWithDiracs );
    PavingWithDiracs pouet;
    P( pouet );
}
