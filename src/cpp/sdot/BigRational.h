#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <tl/support/Displayer.h>
#include <cmath>

namespace sdot {
T_T struct BigRationalFrom;

/**
 * @brief a (slow) rational class represented as num / den * 2 ^ exp
 * 
 */
class BigRational {
public: 
    struct             Normalized {};
    using              BI         = boost::multiprecision::cpp_int;
                        
    /**/               BigRational( Normalized, BI num = 0, BI den = 1, SI64 exp = 0 );
    /**/               BigRational( BI num = 0, BI den = 1, SI64 exp = 0 );
    /**/               BigRational( int num );

    static BigRational from_value ( const auto &value ) { return BigRationalFrom<DECAYED_TYPE_OF( value )>::make( value ); }
           
    void               display    ( Displayer &ds ) const;

    T_T T              to         () const { using std::pow; return T( num ) * pow( T( 2 ), exp ) / T( den ); }

    friend BigRational operator+  ( const BigRational &a, const BigRational &b );
    friend BigRational operator-  ( const BigRational &a, const BigRational &b );
    friend BigRational operator*  ( const BigRational &a, const BigRational &b );
    friend BigRational operator/  ( const BigRational &a, const BigRational &b );

    friend bool        operator== ( const BigRational &a, const BigRational &b );
    friend bool        operator!= ( const BigRational &a, const BigRational &b );
    friend bool        operator<= ( const BigRational &a, const BigRational &b );
    friend bool        operator>= ( const BigRational &a, const BigRational &b );
    friend bool        operator<  ( const BigRational &a, const BigRational &b );
    friend bool        operator>  ( const BigRational &a, const BigRational &b );

    friend BigRational abs        ( const BigRational &a );

    BigRational&       operator+= ( const BigRational &that );
    BigRational&       operator-= ( const BigRational &that );
    BigRational&       operator*= ( const BigRational &that );
    BigRational&       operator/= ( const BigRational &that );

private:
    void               normalize  ();

    BI                 num;
    BI                 den;
    SI64               exp;
};

template<> struct BigRationalFrom<FP64> { static BigRational make( FP64 value ); };

} // namespace sdot