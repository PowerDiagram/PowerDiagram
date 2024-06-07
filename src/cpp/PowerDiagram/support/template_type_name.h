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
#define DECL_BASE_TYPE_INFO_UV( NAME ) template<class U,class V> auto template_type_name( CtType<NAME<U,V>> ) { return #NAME; }
#define DECL_BASE_TYPE_INFO_A_( NAME ) template<class... A> auto template_type_name( CtType<NAME<A...>> ) { return #NAME; }
#define DECL_BASE_TYPE_INFO_T_( NAME ) template<class T> auto template_type_name( CtType<NAME<T>> ) { return #NAME; }

DECL_BASE_TYPE_INFO_T_( std::initializer_list );
DECL_BASE_TYPE_INFO_A_( CtTypeList            );
DECL_BASE_TYPE_INFO_T_( UniquePtr             );
DECL_BASE_TYPE_INFO_T_( CtType                );
DECL_BASE_TYPE_INFO_UV( Pair                  );
DECL_BASE_TYPE_INFO_UVW( Map                   );
DECL_BASE_TYPE_INFO_A_( Tup                   );

#undef DECL_BASE_TYPE_INFO_UV
#undef DECL_BASE_TYPE_INFO_A_
#undef DECL_BASE_TYPE_INFO_T_

// as method
TU auto template_type_name( CtType<U> ) -> decltype( U::template_type_name() ) { return U::template_type_name(); }

// shortcut
TU auto template_type_name() { return template_type_name( CtType<U>() ); }

END_METIL_NAMESPACE
