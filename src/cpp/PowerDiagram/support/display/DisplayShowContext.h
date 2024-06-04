#pragma once

#include "../Vec.h"

BEG_METIL_NAMESPACE
class DisplayItem;

/***/
class DisplayShowContext {
public:
    static Str    port_repr    ( const Vec<Str> &labels );
    static Str    dot_corr     ( StrView str );

    Str           attr_name    () const;
    void          write_td     ( Str content );

    std::ostream* label_stream;
    std::ostream* edge_stream;
    DisplayItem*  root_item;
    Str           ptr_repr;    ///< current ptr_repr
    Vec<Str>      labels;
};

END_METIL_NAMESPACE
