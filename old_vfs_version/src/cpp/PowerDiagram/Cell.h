#pragma once

#include <vfs/Vector.h>
#include <vfs/Point.h>
#include <vfs/List.h>
#include "VtkOutput.h"

/// wrapper around CellImpl
class Cell {
public:
    VfsDtObject_STD_METHODS( Cell, "PowerDiagram", 3 * sizeof( void * ), alignof( void * ) );
    void display_vtk( VtkOutput &vo ) const;
};
