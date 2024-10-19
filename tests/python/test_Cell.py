import PowerDiagram
import numpy

module = PowerDiagram.lib_for( "FP64", 3 )
cell = module.Cell()

def cut( dir ):
    cell.cut_boundary( dir, 1, 10 )
    print( cell.bounded )

cut( [ +1, 0, 0 ] )
cut( [ 0, +1, 0 ] )
cut( [ 0, 0, +1 ] )
cut( [ -1, 0, 0 ] )
cut( [ 0, -1, 0 ] )
cut( [ 0, 0, -1 ] )

print( cell )

vo = module.VtkOutput()
cell.display_vtk( vo )
vo.save( "pd.vtk" )
