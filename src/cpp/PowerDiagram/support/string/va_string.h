#pragma once

#include "to_string.h"

BEG_METIL_NAMESPACE

/// Ex: "... $(0) $(KEY) ...", "84", "KEY:", "smurf" => "... 84 smurf ...". $0 or $KEY work also
std::string va_string_repl_vec( const std::initializer_list<std::string> &values );

/// Ex: "... $(0) $(KEY) ...", val_0, "KEY:", val_KEY => "... 84 smurf ...". $0 or $KEY work also
template<class... A>
std::string va_string( const A &...args ) { return va_string_repl_vec( { to_string( args )... } ); }

END_METIL_NAMESPACE
