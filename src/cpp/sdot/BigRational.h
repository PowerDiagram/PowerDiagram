#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <tl/support/Displayer.h>
#include <Eigen/Dense>
#include <cmath>

namespace sdot {
T_T struct BigRationalInitFrom;

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
    /**/               BigRational( const auto &value ) requires requires ( BI b, SI64 e ) { BigRationalInitFrom<DECAYED_TYPE_OF( value )>::init( b, b, e, value ); } { if ( BigRationalInitFrom<DECAYED_TYPE_OF( value )>::init( num, den, exp, value ) ) normalize(); }

    void               display    ( Displayer &ds ) const;
    explicit operator  bool       () const;

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

    BigRational        operator-  () const;

private:
    void               normalize  ();

    BI                 num;
    BI                 den;
    SI64               exp;
};

template<class I> requires std::is_integral_v<I> struct BigRationalInitFrom<I> {
    static bool init( BigRational::BI &num, BigRational::BI &den, SI64 &exp, const I &value ) {
        num = value;
        den = 1;
        exp = 0;
        return false;
    }
};

template<> struct BigRationalInitFrom<FP64> { static bool init( BigRational::BI &num, BigRational::BI &den, SI64 &exp, FP64 value ); };

} // namespace sdot

namespace Eigen {
    template<> struct NumTraits<sdot::BigRational> : GenericNumTraits<sdot::BigRational> {
        typedef sdot::BigRational NonInteger;
        typedef sdot::BigRational Nested;
        typedef sdot::BigRational Real;

        static inline Real dummy_precision() { return 0; }
        static inline Real digits10() { return 0; }
        static inline Real epsilon() { return 0; }

        enum {
            IsInteger = 0,
            IsSigned = 1,
            IsComplex = 0,
            RequireInitialization = 1,
            ReadCost = 6,
            AddCost = 150,
            MulCost = 100
        };
    };
}
