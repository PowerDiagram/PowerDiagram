#pragma once

#include "CtTypeList.h"
#include "UniquePtr.h"
#include "Pair.h"
#include "Tup.h"

#include <functional>

BEG_METIL_NAMESPACE

/// common types
TTA void for_each_template_arg( CtType<std::function<T(A...)>>, auto &&f ) { f( CtType<T>() ); ( f( CtType<A>() ), ... ); }

#define DECL_BASE_TYPE_INFO_UV( NAME ) template<class U,class V> void for_each_template_arg( CtType<NAME<U,V>>, auto &&f ) { f( CtType<U>() ); f( CtType<V>() ); }
#define DECL_BASE_TYPE_INFO_A_( NAME ) template<class... A> void for_each_template_arg( CtType<NAME<A...>>, auto &&f ) { ( f( CtType<A>() ), ... ); }
#define DECL_BASE_TYPE_INFO_T_( NAME ) template<class T> void for_each_template_arg( CtType<NAME<T>>, auto &&f ) { f( CtType<T>() ); }

DECL_BASE_TYPE_INFO_T_( std::initializer_list );
DECL_BASE_TYPE_INFO_A_( CtTypeList            );
DECL_BASE_TYPE_INFO_T_( UniquePtr             );
DECL_BASE_TYPE_INFO_T_( CtType                );
DECL_BASE_TYPE_INFO_A_( Tup                   );
DECL_BASE_TYPE_INFO_UV( Pair                  );

#undef DECL_BASE_TYPE_INFO_UV
#undef DECL_BASE_TYPE_INFO_A_
#undef DECL_BASE_TYPE_INFO_T_

// as method
TUV auto for_each_template_arg( CtType<U>, V &&f ) -> decltype( U::for_each_template_arg( std::forward<V>( f ) ) ) { return U::for_each_template_arg( std::forward<V>( f ) ); }

// shortcut
TUV auto for_each_template_arg( V &&f ) { return for_each_template_arg( CtType<U>(), std::forward<V>( f ) ); }

END_METIL_NAMESPACE
