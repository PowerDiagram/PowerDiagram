#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/Displayer.h>
#include <tl/support/compare.h>
#include <map>

/**
 * map[ Vec<PII>(...) ] => ... where items in Vec<PII>(...) are strictly increasing
 *
 * This version is for the generic case (it uses std::map which may be slow).
*/
template<int s,class PII=PI32,class PIO=PI32>
class MapOfUniquePISortedArray {
public:
    void  prepare_for( PII max_PI_value ) { values.clear(); }
    PIO&  operator[] ( Vec<PII,s> a ) { auto iter = values.find( a ); if ( iter == values.end() ) iter = values.insert( iter, { a, 0 } ); return iter->second; }
    void  display    ( Displayer &ds ) const { ds << values; }

private:
    using Map        = std::map<Vec<PII,s>,PIO,Less>;

    Map   values;    ///<
};

/// single dim, s == 0
template<class PII,class PIO>
class MapOfUniquePISortedArray<0,PII,PIO> {
public:
    void  prepare_for ( PII /*max_PII_value*/ ) {}
    PIO&  operator[]  ( Vec<PII,0> ) { return value; }
    void  display     ( Displayer &ds ) const { ds << value; }

private:
    PIO   value = 0;  ///<
};

/// single dim, s == 1
template<class PII,class PIO>
class MapOfUniquePISortedArray<1,PII,PIO> {
public:
    void   prepare_for( PII max_PII_value ) { values.resize( max_PII_value, 0 ); }
    PIO&   operator[] ( Vec<PII,1> a ) { return values[ a[ 0 ] ]; }
    void   display    ( Displayer &ds ) const { ds << values; }

private:
    using  Map        = Vec<PIO>;

    Map    values;    ///<
};

/// single dim, s == 2
template<class PII,class PIO>
struct MapOfUniquePISortedArray<2,PII,PIO> {
    void   prepare_for( PI max_PII_value ) { values.resize( ( max_PII_value - 1 ) * max_PII_value / 2, 0 ); }
    PI&    operator[] ( const Vec<PII,2> &a ) { return values[ ( a[ 1 ] - 1 ) * a[ 1 ] / 2 + a[ 0 ] ]; }
    void   display    ( Displayer &ds ) const { ds << values; }

private:
    using  Map        = Vec<PIO>;

    Map    values;    ///<
};
