#include "DisplayPtrId.h"

BEG_METIL_NAMESPACE

DisplayPtrId::DisplayPtrId( const void *ptr ) : id( ptr ? std::to_string( std::size_t( ptr ) ) : "" ) {
}

DisplayPtrId::operator bool() const {
    return ! id.empty();
}

bool DisplayPtrId::operator<( const DisplayPtrId &that ) const {
    return id < that.id;
}

Str DisplayPtrId::graphviz_label() const {
    return "node_" + id;
}

void DisplayPtrId::write( const std::function<void( std::string_view )> &func, DisplayWriteContext &ctx ) const {
    func( id );
}

END_METIL_NAMESPACE
