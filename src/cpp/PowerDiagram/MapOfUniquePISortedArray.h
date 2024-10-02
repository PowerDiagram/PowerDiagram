#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/Displayer.h>
#include <tl/support/compare.h>
#include <map>

/**

*/
template<int s_min,int s_max>
struct MapOfUniquePISortedArray {
    using Tail            = MapOfUniquePISortedArray<s_min,s_max-1>;
    using Head            = MapOfUniquePISortedArray<s_max,s_max>;

    void  set_max_PI_value( PI max_PI_value ) { head.set_max_PI_value( max_PI_value ); tail.set_max_PI_value( max_PI_value ); }

    PI&   operator[]      ( const Vec<PI,s_max> &a ) { return head[ a ]; }
    PI&   operator[]      ( const auto &a ) { return tail[ a ]; }

    void  display         ( Displayer &ds, bool make_array = true ) const { if ( make_array ) ds.start_array(); ds << head; tail.display( ds, false ); if ( make_array ) ds.end_array();}

    Head  head;
    Tail  tail;
};

/// single dim, generic case
template<int s>
struct MapOfUniquePISortedArray<s,s> {
    using Map             = std::map<Vec<PI,s>,PI,Less>;

    void  set_max_PI_value( PI max_PI_value ) { values.clear(); }

    PI&   operator[]      ( Vec<PI,s> a ) { auto iter = values.find( a ); if ( iter == values.end() ) iter = values.insert( iter, { a, 0 } ); return iter->second; }

    void  display         ( Displayer &ds, bool make_array = true ) const { ds << values; }

    Map   values;         ///<
};

/// single dim, s == 0
template<>
struct MapOfUniquePISortedArray<0,0> {
    /**/ MapOfUniquePISortedArray() : value( 0 ) {}
    void set_max_PI_value        ( PI /*max_PI_value*/ ) {}
    PI&  operator[]              ( Vec<PI,0> ) { return value; }
    void display                 ( Displayer &ds, bool make_array = true ) const { ds << value; }

    PI   value;                  ///<
};

/// single dim, s == 1
template<>
struct MapOfUniquePISortedArray<1,1> {
    void    set_max_PI_value( PI max_PI_value ) { values.resize( max_PI_value, 0 ); }
    PI&     operator[]      ( Vec<PI,1> a ) { return values[ a[ 0 ] ]; }
    void    display         ( Displayer &ds, bool make_array = true ) const { ds << values; }

    Vec<PI> values;          ///<
};

/// single dim, s == 2
template<>
struct MapOfUniquePISortedArray<2,2> {
    void    set_max_PI_value( PI max_PI_value ) { values.resize( ( max_PI_value - 1 ) * max_PI_value / 2, 0 ); }
    PI&     operator[]      ( Vec<PI,2> a ) {
        // if ( ( a[ 1 ] - 1 ) * a[ 1 ] / 2 + a[ 0 ] >= values.size() ) {
        //     P( a, values.size() );
        //     TODO;
        // }
        return values[ ( a[ 1 ] - 1 ) * a[ 1 ] / 2 + a[ 0 ] ];
    }
    void    display         ( Displayer &ds, bool make_array = true ) const { ds << values; }

    Vec<PI> values;          ///<
};
