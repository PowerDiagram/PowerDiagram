#pragma once

#include "../common_types.h"

BEG_METIL_NAMESPACE
class DisplayItemFactory;
class DisplayItem_Object;
class DisplayItem;

/***/
class DisplayObjectFiller {
public:
    /**/                DisplayObjectFiller( DisplayItem_Object* object, DisplayItemFactory *ds );

    void                add                ( StrView name, DisplayItem *item ); ///< add a single item
    void                add                ( StrView name, auto&&...items ); ///< add one or more item

private:
    DisplayItem_Object* object;
    DisplayItemFactory* ds;
};

END_METIL_NAMESPACE
