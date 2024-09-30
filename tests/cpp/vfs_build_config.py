import vfs

def config( options ):
    options.load_lib( 'https://github.com/catchorg/Catch2.git', lib_names = [ "Catch2Main", "Catch2" ] )

    # vfs.vfs_build_config( options )

    options.add_cpp_flag( '-march=native' )
    options.add_cpp_flag( '-ffast-math' )
    options.add_cpp_flag( '-O3' )

    options.add_cpp_flag( '-std=c++20' )
    options.add_cpp_flag( '-fPIC' )

    options.add_cpp_flag( '-g3' )

    options.add_inc_path( '../../src/cpp' )
    options.add_inc_path( '../../modules/tl20/src/cpp' )
    options.add_inc_path( '../../modules/amgcl' )
    options.add_inc_path( '../../modules/matplotlib-cpp' )
    options.add_inc_path( '/usr/include/python3.12' )
    options.add_inc_path( '/usr/lib/python3/dist-packages/numpy/core/include' )

    options.add_lib_name( 'python3.12' )
    

