#pragma once

#include "display/DisplayItemFactory.h"
#include "common_types.h"

BEG_METIL_NAMESPACE

/// ct known size version
class SpanOfBits {
public:
    using               Block               = PI;
    using               Byte                = PI8;

    static Str          type_name           () { return "METIL_NAMESPACE::SpanOfBits"; }

    /**/                SpanOfBits          ( const void *data, PI beg, PI end );
    /**/                SpanOfBits          ( const void *data = nullptr, PI end = 0 );

    void                with_aligned_data   ( const std::function<void( const void *data, PI size )> &f, PI alig_in_bits = 8 ) const;
    PI                  nb_bytes            () const;
    DisplayItem*        display             ( DisplayItemFactory &ds, const Str &prefix = {} ) const;
    PI                  size                () const;

    bool                operator[]          ( PI index ) const;
    bool                bit                 ( PI index ) const;

    SpanOfBits          sub_span            ( PI beg, PI end ) const;

    void                copy_to             ( void *dst ) const;

private:
    static constexpr PI nb_bytes_in_a_block = sizeof( Block );
    static constexpr PI nb_bits_in_a_block  = sizeof( Block ) * 8;
    static constexpr PI nb_bits_in_a_byte   = sizeof( Byte ) * 8;

    static Block        block_dirac         ( PI index_in_block );
    static Block        block_range         ( PI beg_index_in_block, PI end_index_in_block );
    static Block        block_up_to         ( PI end_index_in_block );
    static Block        block_from          ( PI beg_index_in_block );

    static Byte         byte_dirac          ( PI index_in_byte );

    const Block*        blocks;
    PI                  beg;
    PI                  end;
};


END_METIL_NAMESPACE
