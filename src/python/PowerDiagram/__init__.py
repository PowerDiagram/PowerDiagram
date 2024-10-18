from pathlib import Path
from munch import Munch
import archspec.cpu
import subprocess
import sys
import os

def get_build_dir( suffix ):
    """
      find a directory to copy .cpp/.h and store .so/.dylib/... files
    """
    
    # try one in the sources (check the write access)
    res = Path( __file__ ).parent / 'bindings' / 'build' / suffix
    if os.access( res, os.W_OK ):
        return res 
    
    # else, try to create it
    try:
        os.makedirs( res )
        return res
    except OSError:
        pass

    # else, make something in the home 
    TODO()


def lib_for( scalar_type, nb_dims ):
    # Prop: on met la librairie à côté, histoire de ne pas avoir de surprise
    # build sert aux .o
    # names
    suffix = f'{ scalar_type }_{ nb_dims }_{ archspec.cpu.host() }'
    module_name = f'PowerDiagram_bindings_for_{ suffix }'

    # where to find/put the files
    build_dir = get_build_dir( suffix )
    if build_dir not in sys.path:
        sys.path.insert( 0, str( build_dir ) )

    # call scons
    ret_code = subprocess.call( [ "scons", f"--sconstruct={ Path( __file__ ).parent / 'bindings' / 'SConstruct' }", '-s',
        f"module_name={ module_name }", 
        f"suffix={ suffix }", 

        f"scalar_type={ scalar_type }",
        f"nb_dims={ nb_dims }"
    ], cwd = str( build_dir ) )
    
    if ret_code:
        raise RuntimeError( "Failed to compile the C++ PowerDiagram binding" )

    # import
    module = __import__( module_name )

    # change names
    res = Munch()
    for n, v in module.__dict__.items():
        if n.endswith( '_' + suffix ):
            n = n[ : -1 - len( suffix ) ]
        res[ n ] = v

    return res

