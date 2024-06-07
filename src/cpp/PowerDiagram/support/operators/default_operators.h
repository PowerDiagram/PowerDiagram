#pragma once

#include "make_array_from_binary_operations.h" // IWYU pragma: export
#include "make_array_from_unary_operations.h" // IWYU pragma: export

#include "../STATIC_ASSERT_IN_IF_CONSTEXPR.h" // IWYU pragma: export
// #include "../on_wrapped_value.h" // IWYU pragma: export
#include "../ScalarClass.h" // IWYU pragma: export
#include "../CtString.h" // IWYU pragma: export
#include "../VoidFunc.h" // IWYU pragma: export
#include "../CtType.h" // IWYU pragma: export
// #include "../get.h" // IWYU pragma: export
// #include "../set.h" // IWYU pragma: export

// #include "../../vfs_system/VfsTypePromoteWrapper.h" // IWYU pragma: export
// #include "../../vfs_system/WrapperTypeFor.h" // IWYU pragma: export

#include <string_view> // IWYU pragma: export
#include <string> // IWYU pragma: export
#include <cmath> // IWYU pragma: export

BEG_METIL_NAMESPACE
struct VfsWrapper;

// needed declarationsn defined elsewhere
Ti constexpr auto ct_value_wrapper_for(); // defined in CtInt.h

//
#define DEFAULT_BIN_OPERATOR_CODE_SIGN( NAME, SIGN ) \
    /* methods */ \
    if constexpr( requires { a.NAME( FORWARD( b ) ); } ) { \
        return a.NAME( FORWARD( b ) ); \
    } else \
    \
    if constexpr( requires { b.r##NAME( FORWARD( a ) ); } ) { \
        return b.r##NAME( FORWARD( a ) ); \
    } else \
    \
    /* ct value */ \
    if constexpr( requires { DECAYED_TYPE_OF( a )::ct_value(); DECAYED_TYPE_OF( b )::ct_value(); } ) { \
        constexpr auto val = DECAYED_TYPE_OF( a )::ct_value() SIGN DECAYED_TYPE_OF( b )::ct_value(); \
        return ct_value_wrapper_for<val>(); \
    } else \
    \
    if constexpr( requires { DECAYED_TYPE_OF( a )::ct_value(); } ) { \
        return DECAYED_TYPE_OF( a )::ct_value() SIGN FORWARD( b ); \
    } else \
    \
    if constexpr( requires { DECAYED_TYPE_OF( b )::ct_value(); } ) { \
        return FORWARD( a ) SIGN DECAYED_TYPE_OF( b )::ct_value(); \
    } else \
\
    /* wrapper */ \
    /*if constexpr( std::is_base_of_v<VfsWrapper,DECAYED_TYPE_OF( a )> ) {*/ \
        /*using Wta = VfsWrapperTypeFor<DECAYED_TYPE_OF( a )>::value;*/ \
        /*using Wtb = VfsWrapperTypeFor<DECAYED_TYPE_OF( b )>::value;*/ \
        /*using Res = VfsTypePromoteWrapper<#NAME,Wta,Wtb>::value;*/ \
        /*return a.template __wrapper_call<Res>( CtString<#NAME>(), a, b );*/ \
    /*} else*/ \
    \
    /*if constexpr( std::is_base_of_v<VfsWrapper,DECAYED_TYPE_OF( b )> ) {*/ \
        /*using Wta = VfsWrapperTypeFor<DECAYED_TYPE_OF( a )>::value;*/ \
        /*using Wtb = VfsWrapperTypeFor<DECAYED_TYPE_OF( b )>::value;*/ \
        /*using Res = VfsTypePromoteWrapper<#NAME,Wta,Wtb>::value;*/ \
        /*return b.template __wrapper_call<Res>( CtString<#NAME>(), a, b );*/ \
    /*} else*/ \
    \
    /* on_wrapped_value( ... ) */ \
    /*if constexpr( requires { on_wrapped_value( FORWARD( a ), [](auto) {} ); } ) {*/ \
    /*     return on_wrapped_value( FORWARD( a ), [&]( auto &&a ) { return FORWARD( a ) SIGN FORWARD( b ); } );*/ \
    /*} else*/ \
    \
    if constexpr( requires { on_wrapped_value( FORWARD( b ), [](auto) {} ); } ) { \
        return on_wrapped_value( FORWARD( b ), [&]( auto &&b ) { return FORWARD( a ) SIGN FORWARD( b ); } ); \
    } else \
    \
    /* arrays */ \
    if constexpr( TensorOrder<DECAYED_TYPE_OF( a )>::value || TensorOrder<DECAYED_TYPE_OF( b )>::value ) { \
        return make_array_from_binary_operations( Functor_##NAME(), FORWARD( a ), FORWARD( b ) ); \
    } else \
    \
    /* scalar_class (a way to avoid recursion... TODO: find something more general and less fragile) */ \
    if constexpr( requires { ScalarClass<DECAYED_TYPE_OF( a )>::value; ScalarClass<DECAYED_TYPE_OF( b )>::value; } ) { \
        return FORWARD( a ) SIGN FORWARD( b ); \
    } else \
    \

// sign means operator like +, *, ... which have to be place between the operands
#define DEFAULT_BIN_SELF_OPERATOR_CODE_SIGN( NAME, SELF_SIGN, SIGN ) \
    /* methods */ \
    if constexpr( requires { a.NAME( FORWARD( b ) ); } ) { \
        return a.NAME( FORWARD( b ) ); \
    } else \
    \
    if constexpr( requires { b.r##NAME( FORWARD( a ) ); } ) { \
        return b.r##NAME( FORWARD( a ) ); \
    } else \
    \
    /* ct value */ \
    if constexpr( requires { DECAYED_TYPE_OF( b )::ct_value(); } ) { \
        return NAME( FORWARD( a ), DECAYED_TYPE_OF( b )::ct_value() ); \
    } else \
    \
    /* wrapper */ \
    /*if constexpr( std::is_base_of_v<VfsWrapper,DECAYED_TYPE_OF( a )> || std::is_base_of_v<VfsWrapper,DECAYED_TYPE_OF( b )> ) {*/ \
        /*using Wta = VfsWrapperTypeFor<DECAYED_TYPE_OF( a )>::value;*/ \
        /*using Wtb = VfsWrapperTypeFor<DECAYED_TYPE_OF( b )>::value;*/ \
        /*using Res = VfsTypePromoteWrapper<#NAME,Wta,Wtb>::value;*/ \
        /*a.template __wrapper_call<void>( CtString<"self_op_pmt__method">(), a, VoidFunc(), Functor_##NAME(), b );*/ \
        /*return PrimitiveCtInt<1>();*/ /*wrapper will always be able to change the internal type*/ \
    /*} else*/ \
    \
    /* get( ... ) */ \
    /*if constexpr( requires { get( FORWARD( b ) ); } ) {*/ \
        /*return NAME( FORWARD( a ), get( FORWARD( b ) ) );*/ \
    /*} else*/ \
    \
    /*if constexpr( requires { set( a, get( a ) SIGN FORWARD( b ) ); } ) {*/ \
        /*return set( a, get( a ) SIGN FORWARD( b ) );*/ \
    /*} else*/ \
    \
    /* scalar_class */ \
    if constexpr( requires { ScalarClass<DECAYED_TYPE_OF( a )>::value; ScalarClass<DECAYED_TYPE_OF( b )>::value; } ) { \
        if constexpr( int( ScalarClass<DECAYED_TYPE_OF( a )>::value ) < int( ScalarClass<DECAYED_TYPE_OF( b )>::value ) ) { \
            return PrimitiveCtInt<0>(); \
        } else { \
            a SELF_SIGN FORWARD( b ); \
            return PrimitiveCtInt<1>(); \
        } \
    } else \
    \

    // /* arrays */ \
    // if constexpr( TensorOrder<DECAYED_TYPE_OF( a )>::value || TensorOrder<DECAYED_TYPE_OF( b )>::value ) { \
    //     return make_array_from_binary_operations( Functor_##NAME(), FORWARD( a ), FORWARD( b ) ); \
    // } else \


//
#define DEFAULT_BIN_OPERATOR_CODE( NAME ) \
    /* methods */ \
    if constexpr( requires { a.NAME( FORWARD( b ) ); } ) { \
            return a.NAME( FORWARD( b ) ); \
    } else \
    \
    if constexpr( requires { b.r##NAME( FORWARD( a ) ); } ) { \
            return b.r##NAME( FORWARD( a ) ); \
    } else \
    \
    /* ct value */ \
    if constexpr( requires { DECAYED_TYPE_OF( a )::ct_value(); DECAYED_TYPE_OF( b )::ct_value(); } ) { \
            constexpr auto val = NAME( DECAYED_TYPE_OF( a )::ct_value(), DECAYED_TYPE_OF( b )::ct_value() ); \
            return ct_value_wrapper_for<val>(); \
    } else \
    \
    if constexpr( requires { DECAYED_TYPE_OF( a )::ct_value(); } ) { \
        return NAME( DECAYED_TYPE_OF( a )::ct_value(), FORWARD( b ) ); \
    } else \
    \
    if constexpr( requires { DECAYED_TYPE_OF( b )::ct_value(); } ) { \
        return NAME( FORWARD( a ), DECAYED_TYPE_OF( b )::ct_value() ); \
    } else \
\
    /* arrays */ \
    if constexpr( TensorOrder<DECAYED_TYPE_OF( a )>::value || TensorOrder<DECAYED_TYPE_OF( b )>::value ) { \
        return make_array_from_binary_operations( Functor_##NAME(), FORWARD( a ), FORWARD( b ) ); \
    } else \


#define DEFAULT_UNA_OPERATOR_CODE( NAME ) \
    /* methods */ \
    if constexpr( requires { a.NAME(); } ) { \
        return a.NAME(); \
    } else \
    \
    /* ct value */ \
    if constexpr( requires { DECAYED_TYPE_OF( a )::ct_value(); } ) { \
        constexpr auto val = NAME( DECAYED_TYPE_OF( a )::ct_value() ); \
        return ct_value_wrapper_for<val>(); \
    } else \
    \
    /* arrays */ \
    if constexpr( TensorOrder<DECAYED_TYPE_OF( a )>::value ) { \
        return make_array_from_unary_operations( Functor_##NAME(), FORWARD( a ) ); \
    } else \
    \


#define DEFAULT_UNA_OPERATOR_CODE_SIGN( NAME, SIGN ) \
    DEFAULT_UNA_OPERATOR_CODE( NAME ) \
    \
    /* scalar_class (a way to avoid recursion... TODO: find something more general and less fragile) */ \
    if constexpr( requires { ScalarClass<DECAYED_TYPE_OF( a )>::value; } ) { \
        return SIGN FORWARD( a ); \
    } else \


END_METIL_NAMESPACE

