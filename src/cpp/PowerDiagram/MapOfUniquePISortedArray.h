#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/compare.h>
#include <map>

/**

*/
template<class T,int s_min,int s_max,class Val>
struct MapOfUniquePISortedArray {
    using Tail      = MapOfUniquePISortedArray<T,s_min,s_max-1,Val>;
    using Head      = MapOfUniquePISortedArray<T,s_max,s_max,Val>;

    void  init      ( PI max_PI_value, const Val &default_value = {} ) { head.init( max_PI_value, default_value ); tail.init( max_PI_value, default_value ); }

    Val&  operator[]( const Vec<T,s_max> &a ) { return head[ a ]; }
    Val&  operator[]( const auto &a ) { return tail[ a ]; }

    void  display   ( Displayer &ds, bool make_array = true ) const { if ( make_array ) ds.start_array(); ds << head; tail.display( ds, false ); if ( make_array ) ds.end_array();}

    Head  head;
    Tail  tail;
};

/// single dim, generic case
template<class T,int s,class Val>
struct MapOfUniquePISortedArray<T,s,s,Val> {
    using Vals           = std::map<Vec<T,s>,Val,Less>;

    void  init           ( PI max_PI_value, const Val &default_value = {} ) { this->default_value = default_value; }

    Val&  operator[]     ( Vec<T,s> a ) { auto iter = values.find( a ); if ( iter == values.end() ) iter = values.insert( iter, { a, default_value } ); return iter->second; }

    void  display        ( Displayer &ds, bool make_array = true ) const { ds << values; }

    Val   default_value; ///<
    Vals  values;        ///<
};


/// single dim, s == -1
template<class T,class Val>
struct MapOfUniquePISortedArray<T,-1,-1,Val> {
    void init             ( PI /*max_PI_value*/, const Val &default_value = {} ) {}
    // Val& operator[]    ( Vec<T,0> ) { TODO; }
};

/// single dim, s == -2
template<class T,class Val>
struct MapOfUniquePISortedArray<T,-2,-2,Val> {
    void init             ( PI /*max_PI_value*/, const Val &default_value = {} ) {}
    // Val& operator[]    ( Vec<T,0> ) { TODO; }
};

/// single dim, s == 0
template<class T,class Val>
struct MapOfUniquePISortedArray<T,0,0,Val> {
    void init             ( PI /*max_PI_value*/, const Val &default_value = {} ) { value = default_value; }

    Val& operator[]       ( Vec<T,0> ) { return value; }

    void display          ( Displayer &ds, bool make_array = true ) const { ds << value; }

    Val  value;           ///<
};

/// single dim, s == 1
template<class T,class Val>
struct MapOfUniquePISortedArray<T,1,1,Val> {
    using Vals            = Vec<Val>;

    void  init            ( PI max_PI_value, const Val &default_value = {} ) { values.resize( max_PI_value ); for( Val &val : values ) val = default_value; }

    Val&  operator[]      ( Vec<T,1> a ) { return values[ a[ 0 ] ]; }

    void  display         ( Displayer &ds, bool make_array = true ) const { ds << values; }

    Vals  values;          ///<
};

/// single dim, s == 2
template<class T,class Val>
struct MapOfUniquePISortedArray<T,2,2,Val> {
    using Vals            = Vec<Val>;

    void  init            ( PI max_PI_value, const Val &default_value = {} ) { values.resize( ( max_PI_value - 1 ) * max_PI_value / 2 ); for( Val &val : values ) val = default_value; }

    Val&  operator[]      ( Vec<T,2> a ) { return values[ ( a[ 1 ] - 1 ) * a[ 1 ] / 2 + a[ 0 ] ]; }

    void  display         ( Displayer &ds, bool make_array = true ) const { ds << values; }

    Vals  values;          ///<
};
