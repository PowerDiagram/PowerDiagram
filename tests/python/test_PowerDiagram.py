import PowerDiagram
import numpy

module = PowerDiagram.lib_for( "FP64", 3 )

# base cell
base_cell = module.Cell()
base_cell.cut_boundary( [ +1, 0, 0 ], 1, 10 )
base_cell.cut_boundary( [ 0, +1, 0 ], 1, 10 )
base_cell.cut_boundary( [ 0, 0, +1 ], 1, 10 )
base_cell.cut_boundary( [ -1, 0, 0 ], 1, 10 )
base_cell.cut_boundary( [ 0, -1, 0 ], 1, 10 )
base_cell.cut_boundary( [ 0, 0, -1 ], 1, 10 )

#
diracs  = module.DiracVecFromLocallyKnownValues( numpy.random.random( [ 2, 3 ] ) )
weights = module.HomogeneousWeights()
paving  = module.RegularGrid( diracs )

print( paving )
# paving.for_each_cell( base_cell, weights, print )

vo = module.VtkOutput()
module.display_vtk( vo, base_cell, paving, weights )
vo.save( "pd.vtk" )

# def cut( dir ):
#     cell.cut_boundary( dir, 1, 10 )
#     print( cell.bounded )

# cut( [ +1, 0, 0 ] )
# cut( [ 0, +1, 0 ] )
# cut( [ 0, 0, +1 ] )
# cut( [ -1, 0, 0 ] )
# cut( [ 0, -1, 0 ] )
# cut( [ 0, 0, -1 ] )

# print( cell )

# cell.display_vtk( vo )
