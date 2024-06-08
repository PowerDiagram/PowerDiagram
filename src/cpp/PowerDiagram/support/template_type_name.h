#pragma once

#include "CtTypeList.h"
#include "UniquePtr.h"
#include "Pair.h"
#include "Tup.h"
#include "Map.h"

#include <functional>

BEG_METIL_NAMESPACE

/// common types
TTA auto template_type_name( CtType<std::function<T(A...)>> ) { return "std::function"; }

#define DECL_BASE_TYPE_INFO_UVW( NAME ) template<class U,class V,class W> auto template_type_name( CtType<NAME<U,V,W>> ) { return #NAME; }
#define DECL_BASE_TYPE_INFO_UV_( NAME ) template<class U,class V> auto template_type_name( CtType<NAME<U,V>> ) { return #NAME; }
#define DECL_BASE_TYPE_INFO_A__( NAME ) template<class... A> auto template_type_name( CtType<NAME<A...>> ) { return #NAME; }
#define DECL_BASE_TYPE_INFO_T__( NAME ) template<class T> auto template_type_name( CtType<NAME<T>> ) { return #NAME; }

DECL_BASE_TYPE_INFO_T__( std::initializer_list );
DECL_BASE_TYPE_INFO_A__( CtTypeList            );
DECL_BASE_TYPE_INFO_T__( UniquePtr             );
DECL_BASE_TYPE_INFO_T__( CtType                );
DECL_BASE_TYPE_INFO_UV_( Pair                  );
DECL_BASE_TYPE_INFO_UVW( Map                   );
DECL_BASE_TYPE_INFO_A__( Tup                   );

#undef DECL_BASE_TYPE_INFO_UVW
#undef DECL_BASE_TYPE_INFO_UV_
#undef DECL_BASE_TYPE_INFO_A__
#undef DECL_BASE_TYPE_INFO_T__

// as method
TU auto template_type_name( CtType<U> ) -> decltype( U::template_type_name() ) { return U::template_type_name(); }

// shortcut
TU auto template_type_name() { return template_type_name( CtType<U>() ); }

END_METIL_NAMESPACE
