#include "PowerDiagram/DiracVecFromLocallyKnownValues.h"
#include "PowerDiagram/PavingWithDiracs.h"
#include "catch_main.h"

using namespace power_diagram;

TEST_CASE( "bench Astro 3D", "" ) {
    auto pos = DiracVecFromLocallyKnownValues::random( 20 );
    auto pd = PavingWithDiracs::New( pos );
    // using PavingWithDiracs = PD_NAME( PavingWithDiracs );
    // PavingWithDiracs pouet;
    // P( pouet );
}
