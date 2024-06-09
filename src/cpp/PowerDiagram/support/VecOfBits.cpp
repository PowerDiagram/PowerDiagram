#include "ByteEndianness.h"
#include "VecOfBits.h"
#include "ASSERT.h"

// #include <sstream>
#include <utility>
#include <cstring>

BEG_METIL_NAMESPACE

PI VecOfBits::nb_full_blocks() const {
    return _size / nb_bits_in_a_block;
}

PI VecOfBits::nb_blocks() const {
    return ( _size + nb_bits_in_a_block - 1u ) / nb_bits_in_a_block;
}

PI VecOfBits::nb_bytes() const {
    return ( _size + nb_bits_in_a_byte - 1u ) / nb_bits_in_a_byte;
}

bool VecOfBits::ext_data() const {
    return _size > local_size;
}

VecOfBits::Block VecOfBits::block_up_to( PI end_index_in_block ) {
    if ( ByteEndianness::host == ByteEndianness::little ) {
        const Block base = end_index_in_block < 8 ? 0ul : ( Block( -1ul ) >> 8 * ( nb_bytes_in_a_block - end_index_in_block / 8 ) );
        const Block byte = ( 0xFF << ( 8 - end_index_in_block % 8 ) ) & 0xFF;
        return base | ( byte << 8 * ( end_index_in_block / 8 ) );
    }
    return end_index_in_block ? ( Block( -1 ) << ( nb_bits_in_a_block - end_index_in_block ) ) : 0ul;
}

VecOfBits::Block VecOfBits::block_from( PI beg_index_in_block ) {
    return ~ block_up_to( beg_index_in_block );
}

VecOfBits::Byte VecOfBits::byte_dirac( PI index_in_byte ) {
    return Byte( 1 ) << ( nb_bits_in_a_block - 1 - index_in_byte );
}

VecOfBits::Block VecOfBits::block_dirac( PI index_in_block ) {
    if ( ByteEndianness::host == ByteEndianness::little )
        return Block( 1 ) << ( 8 * ( index_in_block / 8 ) + ( 7 - index_in_block % 8 ) );
    return Block( 1 ) << ( nb_bits_in_a_block - 1 - index_in_block );
}

VecOfBits::Block VecOfBits::block_range( PI beg_index_in_block, PI end_index_in_block ) {
    return block_from( beg_index_in_block ) & block_up_to( end_index_in_block );
}

PI VecOfBits::nb_bits() const {
    return _size;
}

bool VecOfBits::empty() const {
    return _size == 0;
}

PI VecOfBits::size() const {
    return _size;
}

const VecOfBits::Block &VecOfBits::block( PI num_word ) const {
    return _data[ num_word ];
}

VecOfBits::Block &VecOfBits::block( PI num_word ) {
    return _data[ num_word ];
}

const VecOfBits::Byte &VecOfBits::byte( PI num_byte ) const {
    return reinterpret_cast<const Byte *>( _data )[ num_byte ];
}

VecOfBits::Byte &VecOfBits::byte( PI num_byte ) {
    return reinterpret_cast<Byte *>( _data )[ num_byte ];
}

bool VecOfBits::operator[]( PI index ) const {
    return bit( index );
}

bool VecOfBits::bit( PI index ) const {
    return block( index / nb_bits_in_a_block ) & block_dirac( index % nb_bits_in_a_block );
}

const VecOfBits::Byte *VecOfBits::bytes() const {
    return reinterpret_cast<const Byte *>( _data );
}

VecOfBits::Byte *VecOfBits::bytes() {
    return reinterpret_cast<Byte *>( _data );
}

Str VecOfBits::to_String() const {
    return { reinterpret_cast<const char *>( bytes() ), size() / 8 };
}

VecOfBits::VecOfBits( FromSize, PI size ) {
    _data = size > local_size ?
        new Block[ ( size + nb_bits_in_a_block - 1 ) / nb_bits_in_a_block ] :
        _local_data;
    _size = size;
}

VecOfBits::VecOfBits( SpanOfBits bits ) : VecOfBits( FromSize(), bits.size() ) {
    bits.copy_to( _data );
}

VecOfBits::VecOfBits( FromSizeAndItemValue, PI size, bool default_value ) : VecOfBits( FromSize(), size ) {
    std::memset( _data, default_value ? -1 : 0, nb_bytes() );
}

VecOfBits VecOfBits::from_bytes( PI size, const void *data ) {
    VecOfBits res( FromSize(), size );
    std::memcpy( res.bytes(), data, res.nb_bytes() );
    return res;
}

Str VecOfBits::type_name() {
    return "VecOfBits";
}

VecOfBits::VecOfBits( const VecOfBits &that ) : VecOfBits( FromSize(), that._size ) {
    std::memcpy( bytes(), that.bytes(), that.nb_bytes() );
}

VecOfBits::VecOfBits( VecOfBits &&that ) {
    _size = std::exchange( that._size, 0 );

    if ( _size <= local_size ) {
        std::memcpy( _local_data, that._local_data, nb_bytes() );
        _data = _local_data;
    } else
        _data = std::exchange( that._data, that._local_data );
}

VecOfBits::VecOfBits() {
    _data = _local_data;
    _size = 0;
}

VecOfBits::~VecOfBits() {
    if ( _size > local_size )
        delete [] _data;
}

VecOfBits::operator SpanOfBits() const {
    return { _data, 0, _size };
}

#ifndef AVOID_DISPLAY
DisplayItem *VecOfBits::display( DisplayItemFactory &ds, const Str &prefix ) const {
    std::ostringstream res;
    res << prefix;

    for( PI b = 0; b < nb_bits(); ++b ) {
        if ( b && b % 8 == 0 )
            res << '_';
        res << ( bit( b ) ? '1' : '0' );
    }

    return ds.new_symbol( CtType<VecOfBits>(), res.str() );
}
#endif

bool VecOfBits::operator!=( const VecOfBits &that ) const {
    if ( size() != that.size() )
        return true;
    const PI n = _size / nb_bits_in_a_block, o = _size % nb_bits_in_a_block;
    for( PI i = 0; i < n; ++i )
        if ( _data[ i ] != that._data[ i ] )
            return true;
    if ( o ) {
        Block msk = block_up_to( o );
        if ( ( _data[ n ] & msk ) != ( that._data[ n ] & msk ) )
            return true;
    }
    return false;
}

bool VecOfBits::operator==( const VecOfBits &that ) const {
    if ( size() != that.size() )
        return false;
    const PI n = _size / nb_bits_in_a_block, o = _size % nb_bits_in_a_block;
    for( PI i = 0; i < n; ++i )
        if ( _data[ i ] != that._data[ i ] )
            return false;
    if ( n < nb_blocks() ) {
        Block msk = block_up_to( o );
        if ( ( _data[ n ] & msk ) != ( that._data[ n ] & msk ) )
            return false;
    }
    return true;
}

// VecOfBits VecOfBits::from_display( StrView sv ) {
//     PI s = 0;
//     for( PI i = 0; i < sv.size(); ++i )
//         s += sv[ i ] != '_';

//     VecOfBits res( s );
//     for( PI i = 0, o = 0; i < sv.size(); ++i )
//         if ( sv[ i ] != '_' )
//             res.set_bit( o++, sv[ i ] == '1' );
//     return res;
// }


VecOfBits &VecOfBits::operator=( const VecOfBits &that ) {
    resize_without_copy( that._size );
    std::memcpy( _data, that._data, that.nb_bytes() );
    return *this;
}

VecOfBits &VecOfBits::operator=( VecOfBits &&that ) {
    if ( ext_data() )
        delete [] _data;

    if ( that.ext_data() ) {
        _data = std::exchange( that._data, that._local_data );
        _size = std::exchange( that._size, 0 );
    } else {
        _data = _local_data;
        _size = std::exchange( that._size, 0 );
        std::memcpy( _data, that._local_data, nb_bytes() );
    }

    return *this;
}

VecOfBits VecOfBits::operator<<( PI shift ) const {
    VecOfBits res( FromSize(), shift + nb_bits() );

    if ( PI s = shift % 8 ) {
        // const PI beg_wd = shift / nb_bits_in_a_block, end_wd = ( shift + nb_bits() + nb_bits_in_a_block - 1 ) / nb_bits_in_a_block;
        // const PI sb = shift % nb_bits_in_a_block;

        // // blocks of 0 at the beginning
        // for( PI i = 0; i < beg_wd; ++i )
        //     res.block( i ) = 0;

        // // fast path for aligned offset
        // if ( sb == 0 ) {
        //     for( PI i = beg_wd; i < end_wd; ++i )
        //         res.block( i ) = block( i - beg_wd );
        //     return res;
        // }

        // // write the first word (00xyz => yz000)
        // res.block( beg_wd ) = block( 0 ) << sb;

        // // write middle + last
        // if ( end_wd - beg_wd > nb_blocks() ) {
        //     for( PI i = beg_wd + 1; i < end_wd - 1; ++i )
        //         res.block( i ) = ( block( i - ( beg_wd + 1 ) ) >> ( nb_bits_in_a_block - sb ) ) | ( block( i - beg_wd ) << sb );
        //     res.block( end_wd - 1 ) = block( end_wd - 1 - ( beg_wd + 1 ) ) >> ( nb_bits_in_a_block - sb );
        // } else {
        //     for( PI i = beg_wd + 1; i < end_wd; ++i )
        //         res.block( i ) = ( block( i - ( beg_wd + 1 ) ) >> ( nb_bits_in_a_block - sb ) ) | ( block( i - beg_wd ) << sb );
        // }
        TODO;
    } else {
        // bulk
        std::memcpy( res._data, _data, nb_bytes() );

        // end words (with 0s)
        PI b0 = _size / nb_bits_in_a_block;
        if ( PI e = _size % nb_bits_in_a_block )
            res._data[ b0 ] &= block_up_to( e );
        for( PI i = b0 + 1; i < res.nb_blocks(); ++i )
            res._data[ i ] = 0ul;
    }

    return res;
}

VecOfBits VecOfBits::operator>>( PI n ) const {
    if ( n >= nb_bits() )
        return {};
    return sub_vec( n, nb_bits() );
}

VecOfBits VecOfBits::operator~() const {
    VecOfBits res( FromSize(), _size );
    for( PI i = 0; i < nb_blocks(); ++i )
        res.block( i ) = ~ block( i );
    return res;
}

VecOfBits &VecOfBits::operator&=( const VecOfBits &that ) {
    for( PI i = 0; i < std::min( that.nb_full_blocks(), nb_blocks() ); ++i )
        block( i ) &= that.block( i );
    for( PI i = that.nb_full_blocks(); i < std::min( that.nb_blocks(), nb_blocks() ); ++i )
        block( i ) &= that.block( i ) & block_up_to( that.nb_bits() % nb_bits_in_a_block );
    for( PI i = that.nb_blocks(); i < nb_blocks(); ++i )
        block( i ) = 0ul;
    return *this;
}

VecOfBits &VecOfBits::operator|=( const VecOfBits &that ) {
    for( PI i = 0; i < std::min( that.nb_full_blocks(), nb_blocks() ); ++i )
        block( i ) |= that.block( i );
    for( PI i = that.nb_full_blocks(); i < std::min( that.nb_blocks(), nb_blocks() ); ++i )
        block( i ) |= that.block( i ) & block_up_to( that.nb_bits() % nb_bits_in_a_block );
    return *this;
}

VecOfBits VecOfBits::operator&( const VecOfBits &that ) const {
    VecOfBits res = *this;
    res &= that;
    return res;
}

VecOfBits VecOfBits::operator|( const VecOfBits &that ) const {
    VecOfBits res = *this;
    res |= that;
    return res;
}

void VecOfBits::copy_to( void *data ) const {
    std::memcpy( data, _data, nb_bytes() );
}

void VecOfBits::set_sub_vec_conditional( PI beg, const VecOfBits &value, const VecOfBits &condition ) {
    ASSERT( value.size() == condition.size() );
    if ( beg )
        return set_sub_vec_conditional( 0, value << beg, condition << beg );

    //
    PI n = condition.nb_bits() / nb_bits_in_a_block, o = condition.nb_bits() % nb_bits_in_a_block;
    for( PI i = 0; i < n; ++i )
        block( i ) = ( block( i ) & ~condition.block( i ) ) | ( value.block( i ) & condition.block( i ) );
    if ( o ) {
        const Block mask = condition.block( n ) & block_up_to( o );
        block( n ) = ( block( n ) & ~mask ) | ( value.block( n ) & mask );
    }
}

bool VecOfBits::all_false() const {
    const PI n = nb_bits() / nb_bits_in_a_block, o = nb_bits() % nb_bits_in_a_block;
    for( PI i = 0; i < n; ++i )
        if ( block( i ) )
            return false;
    if ( o )
        if ( block( n ) & block_up_to( o ) )
            return false;
    return true;
}

bool VecOfBits::all_true() const {
    const PI n = nb_bits() / nb_bits_in_a_block, o = nb_bits() % nb_bits_in_a_block;
    for( PI i = 0; i < n; ++i )
        if ( ~ block( i ) )
            return false;
    if ( o )
        if ( ( ~ block( n ) ) & block_up_to( o ) )
            return false;
    return true;
}

void VecOfBits::resize_without_copy( PI new_size ) {
    // TODO: optimize
    *this = { FromSize(), new_size };
}

void VecOfBits::set_false( PI beg, PI end ) {
    if ( beg == end )
        return;

    const PI begn = beg / nb_bits_in_a_block, bego = beg % nb_bits_in_a_block;
    const PI endn = end / nb_bits_in_a_block, endo = end % nb_bits_in_a_block;

    // all on the same word
    if ( begn == endn ) {
        block( begn ) &= ~ block_range( bego, endo );
        return;
    }

    // first word
    block( begn ) &= ~ block_from( bego );

    // middle
    for( PI i = begn + 1; i < endn; ++i )
        block( i ) = 0;

    // last word
    block( endn ) &= ~ block_up_to( endo );
}

void VecOfBits::set_false( PI ind ) {
    byte( ind / nb_bits_in_a_byte ) &= ~ byte_dirac( ind % nb_bits_in_a_byte );
}

void VecOfBits::set_true( PI beg, PI end ) {
    if ( beg == end )
        return;

    const PI begn = beg / nb_bits_in_a_block, bego = beg % nb_bits_in_a_block;
    const PI endn = end / nb_bits_in_a_block, endo = end % nb_bits_in_a_block;

    // all on the same word
    if ( begn == endn ) {
        block( begn ) |= block_range( bego, endo );
        return;
    }

    // first word
    block( begn ) |= block_from( bego );

    // middle
    for( PI i = begn + 1; i < endn; ++i )
        block( i ) = -1ul;

    // last word
    block( endn ) |= block_up_to( endo );
}

void VecOfBits::set_true( PI ind ) {
    byte( ind / nb_bits_in_a_byte ) |= byte_dirac( ind % nb_bits_in_a_byte );
}

void VecOfBits::set_range( PI beg, PI end, bool value ) {
    if ( value )
        return set_true( beg, end );
    return set_false( beg, end );
}

void VecOfBits::set_bit( PI ind, bool value ) {
    if ( value )
        return set_true( ind );
    return set_false( ind );
}

void VecOfBits::set_sub_vec( PI beg, const VecOfBits &value ) {
    const PI end = beg + value.nb_bits();
    ASSERT( end <= nb_bits() );
    if ( value.nb_bits() == 0 )
        return;

    set_range( beg, end, false );
    operator|=( value << beg );
}


VecOfBits VecOfBits::sub_vec( PI beg, PI end ) const {
    if ( beg == end )
        return {};
    ASSERT( end <= nb_bits() );
    ASSERT( beg < end );

    VecOfBits res( FromSize(), end - beg );

    const PI begn = beg / nb_bits_in_a_block, bego = beg % nb_bits_in_a_block;
    const PI endn = end / nb_bits_in_a_block;

    // all on the same block ?
    if ( begn == endn ) {
        res.block( 0 ) = block( begn ) >> bego;
        return res;
    }

    // all the blocks have to be shifted ?
    if ( bego ) {
        // for( PI i = 0; i < res.nb_blocks() - 1; ++i )
        //     res.block( i ) = ( block( first_word_to_take + i ) >> shift ) | ( block( first_word_to_take + i + 1 ) << ( nb_bits_in_a_block - shift ) );
        // res.block( res.nb_blocks() - 1 ) = block( res.nb_blocks() - 1 + first_word_to_take ) >> shift;
        TODO;
        return res;
    }

    for( PI i = 0; i < res.nb_blocks(); ++i )
        res.block( i ) = block( begn + i );

    return res;
}
