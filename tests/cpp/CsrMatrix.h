#pragma once

#include <tl/support/containers/Vec.h>
#include <tl/support/Displayer.h>


/**
 */
template<class TF,class TI=PI> 
class CsrMatrix {
public:
    Vec<Vec<TF>> to_dense() const {
        Vec<Vec<TF>> res( FromSizeAndItemValue(), nb_rows(), FromSizeAndItemValue(), nb_cols(), 0 );
        for( PI r = 0; r < nb_rows(); ++r )
            for( PI o = offsets[ r + 0 ]; o < offsets[ r + 1 ]; ++o )
                res[ r ][ columns[ o ] ] = values[ o ];
        return res;
    }

    void display( Displayer &ds ) const {
        ds << to_dense();
    }

    TI      nb_rows() const { return std::max( offsets.size(), 1ul ) - 1; }
    TI      nb_cols() const { return std::max( offsets.size(), 1ul ) - 1; }

    Vec<TI> offsets;
    Vec<TI> columns;
    Vec<TF> values;
};
