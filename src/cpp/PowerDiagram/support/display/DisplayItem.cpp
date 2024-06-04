#include "DisplayItem.h"

BEG_METIL_NAMESPACE

DisplayItem::DisplayItem( DisplayTypeInfo ti ) : type_info( ti ) {
   src_info = nullptr;
   prev = nullptr;
   next = nullptr;

   nb_use = 0;
}

DisplayItem::~DisplayItem() {
}

END_METIL_NAMESPACE
