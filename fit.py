from pysdot.domain_types import ConvexPolyhedraAssembly
from pysdot import OptimalTransport
import numpy as np

positions = np.random.rand(200,2)
positions[ :, 0 ] *= 0.5

# diracs
for relax in range( 21 ):
    ot = OptimalTransport()
    ot.set_positions(np.array(positions))
    ot.verbosity = 2

    # ot.set_stopping_criterion(1e-8, "max delta masses")
    ot.max_iter = 1

    # solve
    ot.adjust_weights( relax = relax / 20 )
    # ot.adjust_weights()
    print( min( ot.pd.integrals() ) )

    # display
    ot.display_vtk( f"pd{ round( relax ) }.vtk" )
