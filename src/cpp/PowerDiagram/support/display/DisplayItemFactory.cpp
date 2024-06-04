#include "DisplayWriteContext.h"
#include "DisplayShowContext.h"
#include "DisplayItemFactory.h"

#include "DisplayItem_Pointer.h"
#include "DisplayItem_Number.h"
#include "DisplayItem_Symbol.h"
#include "DisplayItem_Object.h"
#include "DisplayItem_String.h"
#include "DisplayItem_Array.h"

#include "graphviz_helpers.h"

#include <fstream>

BEG_METIL_NAMESPACE

Str DisplayItemFactory::include_path() {
    return "metil/support/display/Displayer.h";
}

Str DisplayItemFactory::type_name() {
    return "METIL_NAMESPACE::Displayer";
}

DisplayItem *DisplayItemFactory::new_object( DisplayTypeInfo ti, const std::function<void( DisplayObjectFiller & )> &add_attributes ) {
    auto *res = pool.create<DisplayItem_Object>( ti );
    DisplayObjectFiller dof( res, this );
    add_attributes( dof );
    return res;
}

DisplayItem *DisplayItemFactory::new_array( DisplayTypeInfo ti, const std::function<void( DisplayArrayFiller & )> &add_attributes ) {
    auto *res = pool.create<DisplayItem_Array>( ti );
    DisplayArrayFiller dof( res, this );
    add_attributes( dof );
    return res;
}

void DisplayItemFactory::write_pointers( const std::function<void( std::string_view )> &func, DisplayWriteContext &ctx ) const {
    ctx.sp += "  ";
    for( const auto &p : pointer_map ) {
        if ( p.second->nb_use <= 1 )
            continue;

        if ( ! std::exchange( ctx.on_a_new_line, false ) )
            func( "\n" );
        func( ctx.sp );
        p.first.write( func, ctx );
        func( ": " );
        p.second->write( func, ctx );
    }
    ctx.sp.resize( ctx.sp.size() - 2 );
}

void DisplayItemFactory::show( DisplayItem *root_item ) const {
    Str filename = ".graph.dot";
    std::ofstream os( filename );
    std::ostringstream edge_stream;
    DisplayShowContext ctx{ .label_stream = &os, .edge_stream = &edge_stream, .root_item = root_item };

    os << "digraph SmurfGraphol {\n";

    // root node
    os << "    node_root [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING='4' PORT='_root'><TR>";
    ctx.write_td( root_item->type_info.name );
    ctx.ptr_repr = "node_root";
    root_item->show( ctx );
    os << "</TR></TABLE>>,shape=\"plaintext\"];\n";

    // pointed items
    for( const auto &p : pointer_map ) {
        // context
        ctx.ptr_repr = p.first.graphviz_label();

        // label + edge info
        os << "    " << ctx.ptr_repr << " [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING='4' PORT='_root'><TR>";
        ctx.write_td( p.second->type_info.name );
        p.second->show( ctx );
        os << "</TR></TABLE>>,shape=\"plaintext\"];\n";
    }

    os << edge_stream.str();
    os << "}\n";
    os.close();

    exec_dot( filename );
}

DisplayItem *DisplayItemFactory::new_number( DisplayTypeInfo ti, NumberInfo number ) {
    return pool.create<DisplayItem_Number>( ti, number.numerator );
}

DisplayItem *DisplayItemFactory::new_string( DisplayTypeInfo ti, StrView str ) {
    return pool.create<DisplayItem_String>( ti, str );
}

DisplayItem *DisplayItemFactory::new_symbol( DisplayTypeInfo ti, StrView str ) {
    return pool.create<DisplayItem_Symbol>( ti, str );
}

DisplayItem *DisplayItemFactory::new_pointer( DisplayTypeInfo ti, DisplayPtrId pid, Vec<Str> port_path, Str label, Str style, const std::function<DisplayItem *()> &get_content ) {
    auto *res = pool.create<DisplayItem_Pointer>( ti, pid, port_path, label, style, nullptr );

    if ( pid ) {
        auto iter = pointer_map.find( pid );
        if ( iter == pointer_map.end() ) {
            iter = pointer_map.insert( iter, { pid, nullptr } );
            iter->second = get_content();
        }
        res->content = iter->second;
        if ( iter->second )
            ++iter->second->nb_use;
    }

    return res;
}

// =======================================================================================================================================

DisplayItem *display( DisplayItemFactory &ds, const std::string& str ) { return ds.new_string( CT_DECAYED_TYPE_OF( str ), str ); }
DisplayItem *display( DisplayItemFactory &ds, std::string_view   str ) { return ds.new_string( CT_DECAYED_TYPE_OF( str ), str ); }
DisplayItem *display( DisplayItemFactory &ds, const char*        str ) { return ds.new_string( CT_DECAYED_TYPE_OF( str ), str ); }
DisplayItem *display( DisplayItemFactory &ds, char               str ) { return ds.new_string( CT_DECAYED_TYPE_OF( str ), { &str, 1 } ); }

DisplayItem *display( DisplayItemFactory &ds, PI64               val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }
DisplayItem *display( DisplayItemFactory &ds, PI32               val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }
DisplayItem *display( DisplayItemFactory &ds, PI16               val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }
DisplayItem *display( DisplayItemFactory &ds, PI8                val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }
DisplayItem *display( DisplayItemFactory &ds, SI64               val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }
DisplayItem *display( DisplayItemFactory &ds, SI32               val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }
DisplayItem *display( DisplayItemFactory &ds, SI16               val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }
DisplayItem *display( DisplayItemFactory &ds, SI8                val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }
DisplayItem *display( DisplayItemFactory &ds, bool               val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }
DisplayItem *display( DisplayItemFactory &ds, FP64               val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }
DisplayItem *display( DisplayItemFactory &ds, FP32               val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( val ) } ); }

DisplayItem *display( DisplayItemFactory &ds, const void*        val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( PI( val ) ) } ); }
DisplayItem *display( DisplayItemFactory &ds, void*              val ) { return ds.new_number( CT_DECAYED_TYPE_OF( val ), { .numerator = std::to_string( PI( val ) ) } ); }

END_METIL_NAMESPACE


