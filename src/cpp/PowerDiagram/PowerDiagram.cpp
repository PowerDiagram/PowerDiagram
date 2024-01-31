#include "PowerDiagram.h"

PowerDiagram::PowerDiagram( const List<Point> &pos, const Vector &weights ) {
    set_pos_and_weights( pos, weights );
}

void PowerDiagram::set_pos_and_weights( const List<Point> &pos, const Vector &weights ) {
    P( weights );
    P( pos );
}
