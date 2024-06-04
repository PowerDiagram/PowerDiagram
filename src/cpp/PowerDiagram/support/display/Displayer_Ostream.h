#pragma once

#include "Displayer_Stack.h"
#include <ostream>

BEG_METIL_NAMESPACE

/**
*/
class Displayer_Ostream : public Displayer_Stack {
public:
    /**/          Displayer_Ostream( std::ostream *os );

    virtual void  on_end_item      ( DisplayerItem *item, bool partial ) override;

    std::ostream* os;
};

END_METIL_NAMESPACE
