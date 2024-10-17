# # cpp source directory
# def add_cpp_flags():
#     from pathlib import Path
#     import vfs

#     vfs.add_global_cpp_flag( "-I" + str( Path( __file__ ).parents[ 3 ].joinpath( 'src', 'cpp' ) ) )
#     vfs.add_global_cpp_flag( "--cpp-flag=-O3" )

# add_cpp_flags()

# # exports
# from .ConvexFunction import ConvexFunction
# from .PowerDiagram import PowerDiagram
# load the shared library
# import cppimport
from pathlib import Path
import sys

def test_file( filename, content ):
    pass

def lib( scalar_type, nb_dims ):
    content = """
        module ( pouet ) {
        }
    """
    test_file( str( Path( __file__ ) / "bindings" / f"bindings_{ scalar_type }_{ nb_dims }.cpp" ), content )
    
