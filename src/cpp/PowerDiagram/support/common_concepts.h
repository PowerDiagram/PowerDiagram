#pragma once

#include "common_macros.h"
#include <string_view>

BEG_METIL_NAMESPACE

TT concept HasSizeAndAccess = requires( const T &t ) { t.size(); } && ( requires( const T &t ) { t.begin(); } || requires( const T &t ) { t[ 0 ]; } );
TT concept HasSizeAndSelect = requires( const T &t ) { t.size(); t[ 0 ]; };
TT concept HasBeginAndEnd   = requires( const T &t ) { t.begin(); t.end(); };
TT concept ScalarLike       = requires( const T &t ) { t * t; };
TT concept ListLike         = HasSizeAndSelect<T> || HasBeginAndEnd<T>;
TT concept StrLike          = std::is_convertible_v<T,std::string_view>;
TT concept IteratorLike     = requires( T &t ) { *( t++ ); };
TT concept FunctionLike     = std::is_function_v<std::decay_t<T>>;

END_METIL_NAMESPACE
