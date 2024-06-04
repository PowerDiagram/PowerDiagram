#include "Cell.h"

void Cell::display_vtk( VtkOutput &vo ) const {
    VFS_CALL_METHOD( display_vtk, void, *this, vo );
}
