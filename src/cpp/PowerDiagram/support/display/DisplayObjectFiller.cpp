#include "DisplayObjectFiller.h"
#include "DisplayItem_Object.h"

BEG_METIL_NAMESPACE

DisplayObjectFiller::DisplayObjectFiller( DisplayItem_Object *object, DisplayItemFactory *ds ) : object( object ), ds( ds ) {
}

void DisplayObjectFiller::add( StrView name, DisplayItem *item ) {
    if ( object->last_attr )
        object->last_attr->next = item;
    else
        object->first_attr = item;
    item->prev = object->last_attr;
    object->last_attr = item;

    item->name = name;
}

END_METIL_NAMESPACE

