import vfs

def config( options ):
    options.load_lib( 'https://github.com/catchorg/Catch2.git', lib_names = [ "Catch2Main", "Catch2" ] )

    # vfs.vfs_build_config( options )

    options.add_cpp_flag( '-std=c++20' )
    options.add_cpp_flag( '-fPIC' )
    options.add_cpp_flag( '-g3' )

    options.add_inc_path( '../../src/cpp' )
    

