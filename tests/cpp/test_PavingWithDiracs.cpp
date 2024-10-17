#define ASIMD_DEBUG_ON_OP( NAME, COND, FUNC ) std::cout << FUNC << std::endl;
// #include "PowerDiagram/DiracVecFromLocallyKnownValues.h" 
// #include "PowerDiagram/PavingWithDiracs.h" 
#include "catch_main.h"

// asimd::SimdVec<double> foul_smurf( asimd::SimdVec<double> a, asimd::SimdVec<double> b, asimd::SimdVec<double> c ) {
//     return a * b + c;
// }
void smurf(int) {
}
    
TEST_CASE( "mask", "" ) {
    // using SD = asimd::SimdVec<double>;
    // using SF = asimd::SimdVec<float>;
    // //P( asimd::SimdVec<bool,SV::size()>( SV::iota() > SV( 1 ) ) );
    
    // P( ( SD::iota() > SD( 1 ) ).to_SimdMask() );
    // P( all( SD::iota() > SD( -10 ) ) );
    // P( all( SD::iota() > SD( 1 ) ) );
    // P( any( SD::iota() > SD( 1 ) ) );

    // P( ( SF::iota() > SF( 1 ) ).to_SimdMask() );
    // P( all( SF::iota() > SF( -10 ) ) );
    // P( all( SF::iota() > SF( 1 ) ) );
    // P( any( SF::iota() > SF( 1 ) ) );
    using namespace asimd;

    auto v = SimdVec<FP32,4>::iota( 10 );
    auto w = SimdVec<FP32,4>( 30 ) - v;
    // P( v );
    // P( w );
    // P( v < w );
    // P( as_a_simd_mask( v < w ) );
    // P( as_a_simd_vec( v < w ) );
    //P( smurf( as_a_simd_mask( v < w ) ) );
    P( all( v < 21 ) );
    P( all( v < 15 ) );
    P( all( v <  5 ) );
    P( any( v < 21 ) );
    P( any( v < 15 ) );
    P( any( v <  5 ) );
    // P( any( v < 15 ) );
    P( v < 12 );

    // SimdMask<4,1> n( 1 );
    // P( n );
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
 