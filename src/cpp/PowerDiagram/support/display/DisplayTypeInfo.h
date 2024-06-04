#pragma once

#include "../type_name.h"

BEG_METIL_NAMESPACE

/***/
class DisplayTypeInfo {
public:
    /**/        DisplayTypeInfo( StrView name, bool is_trivial = false ) : is_trivial( is_trivial ), name( name ) {}
    TT          DisplayTypeInfo( CtType<T> ) { is_trivial = ! std::is_polymorphic_v<std::decay_t<T>>; name = type_name<T>(); }
    /**/        DisplayTypeInfo() { is_trivial = true; }

    bool        is_trivial;
    std::string name;
};

END_METIL_NAMESPACE
