#pragma once

#include "../common_types.h"

BEG_METIL_NAMESPACE

/***/
class DisplayWriteContext {
public:
    void  inc_sp        ();
    void  dec_sp        ();

    bool  use_delimiters = false;
    bool  on_a_new_line = true;
    Str   sp;
};

END_METIL_NAMESPACE
