#pragma once

#include "../common_types.h"

BEG_METIL_NAMESPACE

/***/
class DisplayWriteContext {
public:
    void  inc_sp        ();
    void  dec_sp        ();

    bool  on_a_new_line = true;
    bool  compact       = false;
    Str   sp;
};

END_METIL_NAMESPACE
