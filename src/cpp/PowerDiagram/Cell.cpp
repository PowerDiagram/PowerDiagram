#include "Cell.h"

void Cell::init( const Point *orig_point, const Scalar *orig_weight, PI orig_index ) {
    this->orig_weight = orig_weight;
    this->orig_point = orig_point;
    this->orig_index = orig_index;


}
