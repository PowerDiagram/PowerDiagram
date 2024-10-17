// #include "PowerDiagram/DiracVecFromLocallyKnownValues.h" 
// #include "PowerDiagram/PavingWithDiracs.h" 
#include <asimd/SimdVec.h>
#include "catch_main.h"

// asimd::SimdVec<double> foul_smurf( asimd::SimdVec<double> a, asimd::SimdVec<double> b, asimd::SimdVec<double> c ) {
//     return a * b + c;
// }
TEST_CASE( "mask", "" ) {
    using SV = asimd::SimdVec<double>;
    P( asimd::SimdVec<bool,SV::size()>( SV::iota() > SV( 1 ) ) );
}

// using namespace power_diagram;

// TEST_CASE( "bench Astro 3D", "" ) {
//     auto pos = DiracVecFromLocallyKnownValues::random( 32 );
//     auto pd = PavingWithDiracs::New( pos );
//     WeightsWithBounds wwb;
//     Cell base_cell;

//     pd->for_each_cell( base_cell, wwb, [&]( Cell &cell, int num_thread ) {

//     } );

//     // using PavingWithDiracs = PD_NAME( PavingWithDiracs );
//     // PavingWithDiracs pouet;
//     // P( pouet );
// }
 