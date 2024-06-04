#include "Displayer_Ostream.h"

BEG_METIL_NAMESPACE

Displayer_Ostream::Displayer_Ostream( std::ostream *os ) : os( os ) {
}

void Displayer_Ostream::on_end_item( DisplayerItem *item, bool partial ) {
    if ( ! partial ) {
        DisplayerItem::Context cx;
        item->write( [&]( std::string_view str ) { os->write( str.data(), str.size() ); }, cx );
    }
}

END_METIL_NAMESPACE
