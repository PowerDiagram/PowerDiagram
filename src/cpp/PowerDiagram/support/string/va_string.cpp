#include "../common_types.h"
#include "va_string.h"
#include <vector>

BEG_METIL_NAMESPACE
  
static bool is_a_letter( char c ) { return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) || c == '_'; };
static bool is_a_number( char c ) { return c >= '0' && c <= '9'; };
static bool is_alpha( char c ) { return is_a_letter( c ) || is_a_number( c ); };
 
std::string va_string_repl_vec( const std::initializer_list<std::string> &values_ol ) {
    std::vector<std::string> values( values_ol );
    if ( values.empty() )
        return {};

    std::vector<PI> map_inds, num_inds;
    for( PI n = 1; n < values.size(); ++n ) {
        if ( values[ n ].ends_with( ":" ) )
            map_inds.push_back( n++ );
        else
            num_inds.push_back( n );
    }

    auto find_key = [&]( std::string str ) -> const std::string * {
        for( PI i = 0; i < map_inds.size(); ++i )
            if ( values[ map_inds[ i ] ].substr( 0, values[ map_inds[ i ] ].size() - 1 ) == str )
                return &values[ map_inds[ i ] + 1 ];
        return nullptr;
    };

    std::string res;
    const std::string &str = values[ 0 ];
    res.reserve( 2 * str.size() );
    for( PI i = 0; i < str.size(); ++i ) {
        if ( str[ i ] == '$' && ( i == 0 || str[ i - 1 ] != '\\' )  ) {
            // number without ()
            if ( i + 1 < str.size() && is_a_number( str[ i + 1 ] ) ) {
                PI v = 0;
                while( ++i < str.size() && is_a_number( str[ i ] ) )
                    v = 10 * v + ( str[ i ] - '0' );
                if ( v >= num_inds.size() ) {
                    std::cerr << "bad index in va_string " << str << std::endl;
                    assert( 0 );
                }
                res += values[ num_inds[ v ] ];
                --i;
                continue;
            }

            // variable without ()
            if ( i + 1 < str.size() && is_a_letter( str[ i + 1 ] ) ) {
                PI o = i;
                while( ++i < str.size() && is_alpha( str[ i ] ) )
                    ;
                std::string key = str.substr( o, i - o );
                const std::string *val = find_key( key );
                if ( ! val ) {
                    std::cerr << "no key '" << key << "' in va_string " << str << std::endl;
                    assert( 0 );
                }
                res += *val;
                --i;
                continue;
            }

            // ()
            if ( i + 1 < str.size() && str[ i + 1 ] == '(' ) {
                TODO;
            }
        }

        res += str[ i ];
    }

    return res;
}

END_METIL_NAMESPACE
