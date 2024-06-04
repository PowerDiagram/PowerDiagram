#pragma once

#include "display/DisplayItemFactory.h"
#include "common_ctor_selectors.h"
#include "SpanOfBits.h"

BEG_METIL_NAMESPACE

/**
  Vector of bits with big-endian convention (most significant bit first, as in natural numbers)

  Index 0 is then the most significant bit
*/
class VecOfBits {
public:
    using               Block                           = PI;
    using               Byte                            = PI8;

    static VecOfBits    from_bytes                      ( PI size, const void *data );
    static Str          type_name                       ();

    /**/                VecOfBits                       ( const VecOfBits &that );
    /**/                VecOfBits                       ( VecOfBits &&that );

    /**/                VecOfBits                       ( FromSizeAndItemValue, PI size, bool default_value );
    /**/                VecOfBits                       ( FromSize, PI size );
    /**/                VecOfBits                       ( SpanOfBits bits );
    /**/                VecOfBits                       ();

    /**/               ~VecOfBits                       ();

    operator            SpanOfBits                      () const;
    DisplayItem*        display                         ( DisplayItemFactory &ds, const Str &prefix = {} ) const;

    // bool             operator!=                      ( const SpanOfBits &that ) const;
    // bool             operator==                      ( const SpanOfBits &that ) const;

    bool                operator!=                      ( const VecOfBits &that ) const;
    bool                operator==                      ( const VecOfBits &that ) const;

    VecOfBits&          operator&=                      ( const VecOfBits &that ); ///< that is extends by 0s if necessary
    VecOfBits&          operator|=                      ( const VecOfBits &that ); ///< that is extends by 0s if necessary

    VecOfBits&          operator=                       ( const VecOfBits &that );
    VecOfBits&          operator=                       ( VecOfBits &&that );

    VecOfBits           operator<<                      ( PI n ) const;
    VecOfBits           operator>>                      ( PI n ) const;
    VecOfBits           operator&                       ( const VecOfBits &that ) const;
    VecOfBits           operator|                       ( const VecOfBits &that ) const;
    VecOfBits           operator~                       () const;

    PI                  nb_bytes                        () const;
    PI                  nb_bits                         () const;
    bool                empty                           () const;
    PI                  size                            () const;

    bool                all_false                       () const;
    bool                all_true                        () const;

    void                resize_without_copy             ( PI new_size );

    void                set_false                       ( PI beg, PI end );
    void                set_false                       ( PI ind );
    void                set_true                        ( PI beg, PI end );
    void                set_true                        ( PI ind );

    void                set_range                       ( PI beg, PI end, bool value );

    void                copy_to                         ( void *data ) const;

    void                set_sub_vec_conditional         ( PI beg, const VecOfBits &value, const VecOfBits &condition );
    void                set_sub_vec                     ( PI beg, const VecOfBits &value );
    VecOfBits           sub_vec                         ( PI beg, PI end ) const;

    const Byte&         byte                            ( PI num_word ) const;
    Byte&               byte                            ( PI num_word );

    bool                operator[]                      ( PI index ) const;
    void                set_bit                         ( PI index, bool value );
    bool                bit                             ( PI index ) const;

    const Byte*         bytes                           () const;
    Byte*               bytes                           ();

    Str                 to_String                       () const;


private:
    static constexpr PI local_size                      = 64;

    static constexpr PI nb_bytes_in_a_block             = sizeof( Block );
    static constexpr PI nb_bits_in_a_block              = sizeof( Block ) * 8;
    static constexpr PI nb_bits_in_a_byte               = sizeof( Byte ) * 8;
    static constexpr PI nb_bits_in_a_bit                = 1;
    static constexpr PI nb_local_blocks                 = local_size / nb_bits_in_a_block;

    PI                  nb_full_blocks                  () const;
    PI                  nb_blocks                       () const;
    bool                ext_data                        () const;

    static Block        block_dirac                     ( PI index_in_block ); ///< 000...1000... with pos(1) = index_in_block
    static Block        block_range                     ( PI beg_index_in_block, PI end_index_in_block ); ///<
    static Block        block_up_to                     ( PI end_index_in_block ); ///< 111..000.. with nb(1) = end_index_in_block
    static Block        block_from                      ( PI beg_index_in_block ); ///< 000..111.. with nb(0) = beg_index_in_block

    static Byte         byte_dirac                      ( PI index_in_byte ); ///< 000...1000... with pos(1) = index_in_block

    const Block&        block                           ( PI num_word ) const;
    Block&              block                           ( PI num_word );

    Block               _local_data[ nb_local_blocks ]; ///<
    Block*              _data;                          ///<
    PI                  _size;                          ///< nb bits
};

END_METIL_NAMESPACE
