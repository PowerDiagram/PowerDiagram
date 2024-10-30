from .bindings.loader import module_for
import numpy as np

class Cell:
    """
    """

    def __init__( self, ndim = None, dtype = "FP64", _cell = None ):
        if _cell:
            self._binding_module = _cell._binding_module
            self._cell = _cell
        elif ndim:
            self._binding_module = module_for( scalar_type = dtype, nb_dims = ndim )
            self._cell = self._binding_module.Cell()

    def __repr__( self ):
        return self._cell.__repr__()

    @property
    def nb_vertices( self ):
        return self._cell.nb_vertices

    @property
    def nb_cuts( self ):
        return self._cell.nb_cuts

    @property
    def bounded( self ):
        return self._cell.bounded

    @property
    def empty( self ):
        return self._cell.empty
    
    @property
    def true_dimensionality( self ):
        return self._cell.true_dimensionality

    def cut_boundary( self, dir, val, ind = 0 ):
        return self._cell.cut_boundary( np.ascontiguousarray( dir ), val, int( ind ) )

    def display_vtk( self, vtk_output ):
        return self._cell.display_vtk( vtk_output )
