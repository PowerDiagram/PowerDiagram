#include "DisplayWriteContext.h"
#include "DisplayShowContext.h"
#include "DisplayItem_Object.h"

BEG_METIL_NAMESPACE

DisplayItem_Object::DisplayItem_Object( DisplayTypeInfo ti ) : DisplayItem( ti ) {
    first_attr = nullptr;
    last_attr = nullptr;
}

void DisplayItem_Object::write( const std::function<void( StrView )> &func, DisplayWriteContext &ctx ) const {
    ctx.sp += "  ";
    func( type_info.name );
    for( DisplayItem *attr = first_attr; attr; attr = attr->next ) {
        if ( ! std::exchange( ctx.on_a_new_line, false ) )
            func( "\n" );
        func( ctx.sp );
        if ( ! attr->name.empty() ) {
            func( attr->name );
            func( ": " );
        }
        attr->write( func, ctx );
    }
    ctx.sp.resize( ctx.sp.size() - 2 );
}

void DisplayItem_Object::show( DisplayShowContext &ctx ) const {
    for( DisplayItem *attr = first_attr; attr; attr = attr->next ) {
        ctx.labels << attr->name;
        attr->show( ctx );
        ctx.labels.pop_back();
    }
}

END_METIL_NAMESPACE

