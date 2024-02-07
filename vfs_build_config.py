def config( options ):
    # flags for vfs lib
    from vfs import vfs_build_config
    vfs_build_config( options )

    # includes for this project
    options.add_inc_path( "src/cpp" )
