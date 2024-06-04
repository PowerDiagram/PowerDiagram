#pragma once

#include "STATIC_ASSERT_IN_IF_CONSTEXPR.h"
#include "for_each_template_arg.h"
#include "template_type_name.h"
#include "common_macros.h"
#include "common_types.h"

BEG_METIL_NAMESPACE

// qualifiers
TTI inline auto type_name( CtType<const T(&)[i]> ) { return type_name( CtType<const T *>() ); } // send the size ??
TTI inline auto type_name( CtType<T(&)[i]> ) { return type_name( CtType<T *>() ); } // send the size ??

TTI inline auto type_name( CtType<const T[i]> ) { return type_name( CtType<const T *>() ); } // send the size ??
TTI inline auto type_name( CtType<T[i]> ) { return type_name( CtType<T *>() ); } // send the size ??

TT  inline auto type_name( CtType<const T> ) { return Str( "const " ) + type_name( CtType<T>() ); }
TT  inline auto type_name( CtType<T &&> ) { return Str( type_name( CtType<T>() ) ) + " &&"; }
TT  inline auto type_name( CtType<T &> ) { return Str( type_name( CtType<T>() ) ) + " &"; }
TT  inline auto type_name( CtType<T *> ) { return Str( type_name( CtType<T>() ) ) + " *"; }

// common types
#define DECL_TYPE_NAME( NAME ) inline auto type_name( CtType<NAME> ) { return #NAME; }
DECL_TYPE_NAME( PI64    );
DECL_TYPE_NAME( PI32    );
DECL_TYPE_NAME( PI16    );
DECL_TYPE_NAME( PI8     );

DECL_TYPE_NAME( SI64    );
DECL_TYPE_NAME( SI32    );
DECL_TYPE_NAME( SI16    );
DECL_TYPE_NAME( SI8     );

DECL_TYPE_NAME( Bool    );
DECL_TYPE_NAME( Byte    );

DECL_TYPE_NAME( FP64    );
DECL_TYPE_NAME( FP32    );

DECL_TYPE_NAME( char    );

DECL_TYPE_NAME( void    );

DECL_TYPE_NAME( StrView );
DECL_TYPE_NAME( Str     );
#undef DECL_DECL_TYPE_NAME

// special cases
template<class T,class... A> Str type_name( CtType<std::function<T(A...)>> ) {
    Str res;
    ( ( res += ( res.empty() ? "" : "," ) + type_name( CtType<A>() ) ), ... );
    return Str( "std::function<" ) + type_name( CtType<T>() ) + "(" + res + ")>";
}

// helpers for the generic version
TT auto CtType<T>::to_string() { return type_name( CtType<T>() ); }
TT auto *CtType<T>::display( auto &ds ) { return ds.string( to_string() ); }

// generic version
TT Str type_name( CtType<T> ) {
    if constexpr( requires { T::type_name(); } ) {
        return T::type_name();
    } else if constexpr( requires { template_type_name( CtType<T>() ); } ) {
        Str res = template_type_name( CtType<T>() );
        if constexpr( requires { for_each_template_arg( CtType<T>(), []( auto ) {} ); } ) {
            res += "<";
            PI nb_template_args = 0;
            for_each_template_arg( CtType<T>(), [&]( auto n ) {
                if ( nb_template_args++ )
                    res += ",";
                res += n.to_string();
            } );
            res += ">";
        }
        return res;
    } else
        STATIC_ASSERT_WITH_RETURN_IN_IF_CONSTEXPR( "", 0, "found no way to get type_name" );
}

//
// TT DisplayerItem *display( Displayer &ds, CtType<T> v ) { return ds.string( type_name( v ) ); }

// shortcut type_name<T>()
TT Str type_name() { return type_name( CtType<T>() ); }

#define STD_METIL_TYPE_INFO( CLASS_NAME, INCLUDE_PATH, ... ) \
    public: \
        void for_each_attribute( auto &&f ) const { parse_attributes( f, #__VA_ARGS__ , ##__VA_ARGS__ ); } \
        void for_each_attribute( auto &&f ) { parse_attributes( f, #__VA_ARGS__ , ##__VA_ARGS__ ); } \
        static Str template_type_name() { return #CLASS_NAME; } \

#define STD_METIL_TYPE_INFO_VIRT( CLASS_NAME, INCLUDE_PATH, ... ) \
    public: \
        void for_each_attribute( auto &&f ) const { parse_attributes( f, #__VA_ARGS__ , ##__VA_ARGS__ ); } \
        void for_each_attribute( auto &&f ) { parse_attributes( f, #__VA_ARGS__ , ##__VA_ARGS__ ); } \
        virtual DisplayItem *display( DisplayItemFactory &ds ) const override { return ds.new_object( *this ); } \
        static Str template_type_name() { return #CLASS_NAME; } \

END_METIL_NAMESPACE
