#include "DisplayShowContext.h"
#include "DisplayItem_Number.h"

BEG_METIL_NAMESPACE

DisplayItem_Number::DisplayItem_Number( DisplayTypeInfo ti, std::string numerator ) : DisplayItem( ti ), numerator( numerator ) {
}

void DisplayItem_Number::write( const std::function<void( std::string_view )> &func, DisplayWriteContext &ctx ) const {
    func( numerator );
}

void DisplayItem_Number::show( DisplayShowContext &ctx ) const {
    ctx.write_td( numerator );
}

END_METIL_NAMESPACE

