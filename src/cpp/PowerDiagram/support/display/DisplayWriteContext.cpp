#include "DisplayWriteContext.h"

BEG_METIL_NAMESPACE

void DisplayWriteContext::inc_sp() {
    sp += "  ";
}

void DisplayWriteContext::dec_sp() {
    sp.resize( sp.size() - 2 );
}

END_METIL_NAMESPACE

