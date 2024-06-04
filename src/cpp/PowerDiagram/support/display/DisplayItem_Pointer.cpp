#include "DisplayItem_Pointer.h"
#include "DisplayWriteContext.h"
#include "DisplayShowContext.h"
#include "../TODO.h"

BEG_METIL_NAMESPACE

DisplayItem_Pointer::DisplayItem_Pointer( DisplayTypeInfo ti, DisplayPtrId pointer_id, Vec<Str> port_path, Str label, Str style, DisplayItem *content ) : DisplayItem( ti ), pointer_id( pointer_id ), port_path( port_path ), content( content ), label( label ), style( style ) {
}

void DisplayItem_Pointer::write( const std::function<void( StrView )> &func, DisplayWriteContext &ctx ) const {
    if ( ! content )
        return func( "NULL" );

    if ( content->nb_use > 1 )
        return func( pointer_id.id );

    ctx.inc_sp();
    content->write( func, ctx );
    ctx.dec_sp();
}

void DisplayItem_Pointer::show( DisplayShowContext &ctx ) const {
    ctx.write_td( "" );

    if ( pointer_id ) {
        std::ostream &es = *ctx.edge_stream;
        es << "    " << ctx.ptr_repr;
        if ( ctx.labels.size() )
             es << ":" << ctx.attr_name() << "";

        es << " -> " << pointer_id.graphviz_label();
        if ( port_path.size() )
            es << ":" << ctx.port_repr( port_path ) << "";

        int cpt = 0;
        if ( style.size() )
            es << ( cpt++ ? "," : "[" ) << "style=\"" << style << "\"";
        if ( label.size() )
            es << ( cpt++ ? "," : "[" ) << "label=\"" << ctx.dot_corr( label ) << "\"";
        if ( cpt )
            es << "]";
        es << "\n";
    }
}

END_METIL_NAMESPACE

