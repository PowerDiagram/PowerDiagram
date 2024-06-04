#include "PowerDiagram.h"

PowerDiagram::PowerDiagram( PtPtr &&point_tree ) : point_tree( std::move( point_tree ) ) {
}

PowerDiagram::~PowerDiagram() {
}

DisplayItem *PowerDiagram::display( DisplayItemFactory &df ) const {
    return df.new_display_item( *point_tree );
}
