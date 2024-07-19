#include "DisplayWriteContext.h"
#include "DisplayShowContext.h"
#include "DisplayItem_Array.h"

BEG_METIL_NAMESPACE

DisplayItem_Array::DisplayItem_Array( DisplayTypeInfo ti ) : DisplayItem( ti ) {
    first_attr = nullptr;
    last_attr = nullptr;
}

bool DisplayItem_Array::need_cr( DisplayWriteContext &ctx ) const {
    for( DisplayItem *attr = first_attr; attr; attr = attr->next ) {
        if ( attr->need_cr( ctx ) )
            return true;
    }
    return false;
}

void DisplayItem_Array::write( const std::function<void( StrView )> &func, DisplayWriteContext &ctx ) const {
    if ( need_cr( ctx ) && ! ctx.compact ) {
        ctx.inc_sp();
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
        ctx.dec_sp();
    } else {
        func( "[" );
        for( DisplayItem *attr = first_attr; attr; attr = attr->next ) {
            if ( attr != first_attr )
                func( "," );
            func( " " );

            if ( ! attr->name.empty() ) {
                func( attr->name );
                func( ": " );
            }
            attr->write( func, ctx );
        }
        if ( first_attr )
            func( " " );
        func( "]" );
    }
}

void DisplayItem_Array::show( DisplayShowContext &ctx ) const {
    PI cpt = 0;
    *ctx.label_stream << "<TD BGCOLOR='red' WIDTH='2' CELLPADDING='0' PORT='" << ctx.attr_name() << "'></TD>";
    for( DisplayItem *attr = first_attr; attr; attr = attr->next ) {
        ctx.labels << std::to_string( cpt++ );
        attr->show( ctx );
        ctx.labels.pop_back();
    }
    *ctx.label_stream << "<TD BGCOLOR='blue' WIDTH='2' CELLPADDING='0'></TD>";
}

END_METIL_NAMESPACE

