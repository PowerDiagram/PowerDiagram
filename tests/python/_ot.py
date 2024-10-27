from pysdot import OptimalTransport
import numpy as np

positions = np.array( [
    [ 0.00, 0.5 ],
    [ 0.01, 0.5 ],
    [ 0.02, 0.5 ],
    [ 0.97, 0.5 ],
    [ 0.98, 0.5 ],
    [ 0.99, 0.5 ],
] )

# diracs
ot = OptimalTransport( positions )
ot.verbosity = 2

# solve
ot.adjust_weights()

# display
ot.display_vtk( "pd.vtk" )

# print( ot.pd.display_html() )

