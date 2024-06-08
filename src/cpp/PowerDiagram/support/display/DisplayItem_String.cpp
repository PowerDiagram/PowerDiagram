#include "DisplayShowContext.h"
#include "DisplayItem_String.h"

BEG_METIL_NAMESPACE

DisplayItem_String::DisplayItem_String( DisplayTypeInfo ti, StrView content ) : DisplayItem( ti ), content( content ) {
}

bool DisplayItem_String::need_cr( DisplayWriteContext &ctx ) const {
    return false;
}

void DisplayItem_String::write( const std::function<void( StrView )> &func, DisplayWriteContext &ctx ) const {
    func( "\"" );
    func( content );
    func( "\"" );
}

void DisplayItem_String::show( DisplayShowContext &ctx ) const {
    ctx.write_td( '"' + content + '"' );
}

END_METIL_NAMESPACE

