#pragma once

#include "../metil_namespace.h"

BEG_METIL_NAMESPACE
class DisplayItemFactory;
class DisplayItem_Array;
class DisplayItem;

/***/
class DisplayArrayFiller {
public:
    /**/                DisplayArrayFiller( DisplayItem_Array* array, DisplayItemFactory *ds );

    void                add               ( DisplayItem *item ); ///<
    void                add               ( auto &&item ); ///<

private:
    DisplayItem_Array*  array;
    DisplayItemFactory* ds;
};

END_METIL_NAMESPACE
