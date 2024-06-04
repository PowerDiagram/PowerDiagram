#include "DisplayShowContext.h"
#include <ostream>

BEG_METIL_NAMESPACE

Str DisplayShowContext::port_repr( const Vec<Str> &labels ) {
    auto is_a_number = []( char c ) { return c >= '0' && c <= '9'; };

    Str res;
    for( const Str &label : labels ) {
        if ( res.size() )
            res += "_";
        else if ( label.size() && is_a_number( label[ 0 ] ) )
            res += "_";

        for( char c : label ) {
            switch ( c ) {
            case '&': res += "_e"; break;
            case '-': res += "_m"; break;
            case '>': res += "_g"; break;
            case '<': res += "_l"; break;
            case '_': res += "__"; break;
            case ' ': res += "_s"; break;
            case '[': res += "_o"; break;
            case ']': res += "_c"; break;
            case '(': res += "_O"; break;
            case ')': res += "_C"; break;
            case '.': res += "_d"; break;
            default: res += c;
            }
        }
    }

    return res;
}

Str DisplayShowContext::dot_corr( StrView str ) {
    return Str{ str };
}

Str DisplayShowContext::attr_name() const {
    return port_repr( labels );
}

void DisplayShowContext::write_td( Str content ) {
    *label_stream << "<TD port='" << attr_name() << "'>" << content << "</TD>";
}

END_METIL_NAMESPACE
