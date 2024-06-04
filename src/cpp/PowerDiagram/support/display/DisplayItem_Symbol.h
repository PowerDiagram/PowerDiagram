#pragma once

#include "DisplayItem.h"

BEG_METIL_NAMESPACE

/***/
class DisplayItem_Symbol : public DisplayItem {
public:
    /**/         DisplayItem_Symbol( DisplayTypeInfo ti, StrView content );
    
    virtual void write             ( const std::function<void( StrView str )> &func, DisplayWriteContext &ctx ) const override;
    virtual void show              ( DisplayShowContext &ctx ) const override;

    Str          content;
};

END_METIL_NAMESPACE
