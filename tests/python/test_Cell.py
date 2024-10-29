from sdot import Cell
import numpy

def test_Cell_emptyness_bounded():
    c = Cell( ndim = 2 )
    assert c.nb_vertices == 0
    assert c.bounded == False
    assert c.empty == False

    c.cut_boundary( [ +1, 0 ], 1 )
    assert c.nb_vertices == 0
    assert c.bounded == False
    assert c.empty == False

    c.cut_boundary( [ 0, +1 ], 1 )
    assert c.nb_vertices == 1
    assert c.bounded == False
    assert c.empty == False

    c.cut_boundary( [ -1, 0 ], 1 )
    assert c.nb_vertices == 2
    assert c.bounded == False
    assert c.empty == False

    c.cut_boundary( [ 0, -1 ], 1 )
    assert c.nb_vertices == 4
    assert c.bounded == True
    assert c.empty == False

    c.cut_boundary( [ 1, 0 ], 0.5 )
    assert c.nb_vertices == 4
    assert c.bounded == True
    assert c.empty == False

    c.cut_boundary( [ 1, 0 ], -2 )
    assert c.nb_vertices == 0
    assert c.bounded == True
    assert c.empty == True

    c.cut_boundary( [ 1, 0 ], -3 )
    assert c.nb_vertices == 0
    assert c.bounded == True
    assert c.empty == True

def test_Cell_emptyness_unbounded():
    c = Cell( ndim = 3 )
    assert c.nb_vertices == 0
    assert c.bounded == False
    assert c.empty == False

    c.cut_boundary( [ 1, 1, 0 ], 1 )
    print( "nb_vertices", c.nb_vertices )
    print( "bounded", c.bounded )
    print( "empty", c.empty )

    c.cut_boundary( [ -1, 0, 0 ], 0 )
    print( "nb_vertices", c.nb_vertices )
    print( "bounded", c.bounded )
    print( "empty", c.empty )

    c.cut_boundary( [ 0, -1, 0 ], 0 )
    print( "nb_vertices", c.nb_vertices )
    print( "bounded", c.bounded )
    print( "empty", c.empty )

    # assert c.nb_vertices == 0
    # assert c.bounded == False
    # assert c.empty == False

    # c.cut_boundary( [ 0, -1 ], 0 )
    # assert c.nb_vertices == 1
    # assert c.bounded == False
    # assert c.empty == True

# test_Cell_emptyness_bounded()
test_Cell_emptyness_unbounded()

# cut( [ +1, 0, 0 ] )
# cut( [ 0, +1, 0 ] )
# cut( [ 0, 0, +1 ] )
# cut( [ -1, 0, 0 ] )
# cut( [ 0, -1, 0 ] )
# cut( [ 0, 0, -1 ] )

# print( cell )

# vo = module.VtkOutput()
# cell.display_vtk( vo )
# vo.save( "pd.vtk" )
