import PowerDiagram
import numpy

module = PowerDiagram.lib_for( "FP64", 3 )

cell = module.Cell()

cell.cut_boundary( [ 1, 0, 0 ], 1, 10 )

print( cell )
# print( cell.nb_vertices )

