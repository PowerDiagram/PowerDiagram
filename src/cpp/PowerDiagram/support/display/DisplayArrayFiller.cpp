#include "DisplayArrayFiller.h"
#include "DisplayItem_Array.h"

BEG_METIL_NAMESPACE

DisplayArrayFiller::DisplayArrayFiller( DisplayItem_Array *array, DisplayItemFactory *ds ) : array( array ), ds( ds ) {
}

void DisplayArrayFiller::add( DisplayItem *item ) {
    if ( array->last_attr )
        array->last_attr->next = item;
    else
        array->first_attr = item;
    item->prev = array->last_attr;
    array->last_attr = item;
}

END_METIL_NAMESPACE

