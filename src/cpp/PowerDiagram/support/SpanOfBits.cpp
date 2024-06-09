#include "ByteEndianness.h"
#include "SpanOfBits.h"
#include "ASSERT.h"
#include <sstream>
#include <cstring>

BEG_METIL_NAMESPACE

SpanOfBits::Block SpanOfBits::block_up_to( PI end_index_in_block ) {
    if ( ByteEndianness::host == ByteEndianness::little ) {
        const Block base = end_index_in_block < 8 ? 0ul : ( Block( -1ul ) >> 8 * ( nb_bytes_in_a_block - end_index_in_block / 8 ) );
        const Block byte = ( 0xFF << ( 8 - end_index_in_block % 8 ) ) & 0xFF;
        return base | ( byte << 8 * ( end_index_in_block / 8 ) );
    }
    return end_index_in_block ? ( Block( -1 ) << ( nb_bits_in_a_block - end_index_in_block ) ) : 0ul;
}

SpanOfBits::Block SpanOfBits::block_from( PI beg_index_in_block ) {
    return ~ block_up_to( beg_index_in_block );
}

SpanOfBits::Byte SpanOfBits::byte_dirac( PI index_in_byte ) {
    return Byte( 1 ) << ( nb_bits_in_a_byte - 1 - index_in_byte );
}

SpanOfBits::Block SpanOfBits::block_dirac( PI index_in_block ) {
    if ( ByteEndianness::host == ByteEndianness::little )
        return Block( 1 ) << ( 8 * ( index_in_block / 8 ) + ( 7 - index_in_block % 8 ) );
    return Block( 1 ) << ( nb_bits_in_a_block - 1 - index_in_block );
}

SpanOfBits::Block SpanOfBits::block_range(PI beg_index_in_block, PI end_index_in_block) {
    return block_from( beg_index_in_block ) & block_up_to( end_index_in_block );
}

SpanOfBits::SpanOfBits( const void *data, PI beg, PI end ) : blocks( reinterpret_cast<const Block *>( data ) ), beg( beg ), end( end ) {
}

SpanOfBits::SpanOfBits( const void *data, PI end ) : SpanOfBits( data, 0, end ) {
}

void SpanOfBits::with_aligned_data( const std::function<void( const void *, PI )> &f, PI alig_in_bits ) const {
    ASSERT( alig_in_bits >= 8 );
    if ( beg % alig_in_bits ) {
        TODO;
        return;
    }
    
    f( reinterpret_cast<const PI8 *>( blocks ) + beg / 8, size() );
}

#ifndef AVOID_DISPLAY
DisplayItem *SpanOfBits::display( DisplayItemFactory &ds, const Str &prefix ) const {
    std::ostringstream res;
    res << prefix;

    for( PI b = 0; b < size(); ++b ) {
        if ( b && b % 8 == 0 )
            res << '_';
        res << ( bit( b ) ? '1' : '0' );
    }

    return ds.new_symbol( CtType<SpanOfBits>(), res.str() );
}
#endif

PI SpanOfBits::nb_bytes() const {
    return ( size() + 7 ) / 8;
}

bool SpanOfBits::bit( PI index ) const {
    const PI o = beg + index;
    return reinterpret_cast<const Byte *>( blocks )[ o / 8 ] & byte_dirac( o % 8 );
}

bool SpanOfBits::operator[]( PI index ) const {
    return bit( index );
}

PI SpanOfBits::size() const {
    return end - beg;
}

SpanOfBits SpanOfBits::sub_span( PI beg, PI end ) const {
    return { blocks, this->beg + beg, this->beg + end };
}

void SpanOfBits::copy_to( void *dst ) const {
    if ( beg % 8 )
        TODO;
    std::memcpy( dst, reinterpret_cast<const Byte *>( blocks ) + beg / 8, nb_bytes() );
}

END_METIL_NAMESPACE
