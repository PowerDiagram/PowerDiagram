#pragma once

#include "DisplayPtrId.h"
#include "DisplayItem.h"
#include "../Vec.h"

BEG_METIL_NAMESPACE

/***/
class DisplayItem_Pointer : public DisplayItem {
public:
    /**/         DisplayItem_Pointer( DisplayTypeInfo ti, DisplayPtrId pointer_id, Vec<Str> port_path, Str label, Str style, DisplayItem *content );
    
    virtual bool need_cr            ( DisplayWriteContext &ctx ) const override;
    virtual void write              ( const std::function<void( StrView str )> &func, DisplayWriteContext &ctx ) const override;
    virtual void show               ( DisplayShowContext &ctx ) const override;

    DisplayPtrId pointer_id;        ///<
    Vec<Str>     port_path;         ///<
    DisplayItem* content;           ///<
    Str          label;             ///<
    Str          style;             ///<
};

END_METIL_NAMESPACE
