#include "PowerDiagram.h"

PowerDiagram::PowerDiagram() {
}

void PowerDiagram::set_pos_and_weights( const List<Point> &pos, const Vector &weights ) {
    P( weights );
    P( pos );
}
