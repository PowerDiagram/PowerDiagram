#pragma once

#include "default_operators.h"

BEG_METIL_NAMESPACE

// ///
// struct Functor_max {
//     auto operator()( auto &&...a ) const;
// };

// /// binary
// constexpr auto max( auto &&a, auto &&b ) {
//     DEFAULT_BIN_OPERATOR_CODE( max )

//     return a >= b ? FORWARD( a ) : FORWARD( b );
// }

// /// more than 2 operands
// constexpr auto max( auto &&a, auto &&b, auto &&c, auto &&...d ) {
//     return max( max( FORWARD( a ), FORWARD( b ) ), FORWARD( c ), FORWARD( d )... );
// }

// /// seq max
// auto max( auto &&a ) requires ( TensorOrder<DECAYED_TYPE_OF( a )>::value == 1 ) {
//     using std::max;
//     auto res = a[ 0 ];
//     for( std::size_t i = 1; i < a.size(); ++i )
//         res = max( res, a[ i ] );
//     return res;
// }

// ///
// auto Functor_max::operator()( auto &&...a ) const { return max( FORWARD( a )... ); }

END_METIL_NAMESPACE
