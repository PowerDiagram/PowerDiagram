#pragma once

#include "DisplayItem.h"
#include <string>

BEG_METIL_NAMESPACE

/***/
class DisplayItem_Number : public DisplayItem {
public:
    /**/         DisplayItem_Number( DisplayTypeInfo ti, std::string numerator );
    
    virtual bool need_cr           ( DisplayWriteContext &ctx ) const override;
    virtual void write             ( const std::function<void( std::string_view str )> &func, DisplayWriteContext &ctx ) const override;
    virtual void show              ( DisplayShowContext &ctx ) const override;

    std::string  numerator;
};

END_METIL_NAMESPACE
