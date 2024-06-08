#pragma once

#include "DisplayItem.h"

BEG_METIL_NAMESPACE

/***/
class DisplayItem_Object : public DisplayItem {
public:
    /**/         DisplayItem_Object( DisplayTypeInfo ti );
    
    virtual bool need_cr           ( DisplayWriteContext &ctx ) const override;
    virtual void write             ( const std::function<void( StrView str )> &func, DisplayWriteContext &ctx ) const override;
    virtual void show              ( DisplayShowContext &ctx ) const override;

    DisplayItem* first_attr;       ///<
    DisplayItem* last_attr;        ///<
};

END_METIL_NAMESPACE
