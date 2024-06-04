#include "DisplayShowContext.h"
#include "DisplayItem_Symbol.h"
#include "../TODO.h"

BEG_METIL_NAMESPACE

DisplayItem_Symbol::DisplayItem_Symbol( DisplayTypeInfo ti, StrView content ) : DisplayItem( ti ), content( content ) {
}

void DisplayItem_Symbol::write( const std::function<void( StrView )> &func, DisplayWriteContext &ctx ) const {
    func( content );
}

void DisplayItem_Symbol::show( DisplayShowContext &ctx ) const {
    ctx.write_td( content );
}

END_METIL_NAMESPACE

