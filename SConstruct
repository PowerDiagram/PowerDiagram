def make_env():
    # StaticLibrary('foo', ['f1.c', 'f2.c', 'f3.c'])

    # CPPPATH
    with open( ".PowerDiagram.includes", 'r' ) as f:
        CPPPATH = list( filter( bool, f.read().split( "\n" ) ) )

    # CXXFLAGS
    CXXFLAGS = [
        '-std=c++20',

        # '-march=skylake',
        '-march=native',
        '-ffast-math',
        '-O3',

        '-g3',
    ]

    # LIBS
    LIBS = [
        'Catch2Main',
        'Catch2',
    ]

    # LIBPATH
    LIBPATH = [ '/home/leclerc/.vfs_build/ext/Catch2/install/lib' ]

    return Environment( CPPPATH = CPPPATH, CXXFLAGS = CXXFLAGS, LIBS = LIBS, LIBPATH = LIBPATH )

# 
libs = [
    "modules/tl20/src/cpp/tl/support/display/DisplayParameters.cpp",
    "modules/tl20/src/cpp/tl/support/display/DisplayItem_Number.cpp",
    "modules/tl20/src/cpp/tl/support/display/DisplayItem_Pointer.cpp",
    "modules/tl20/src/cpp/tl/support/display/DisplayItem_String.cpp",
    "modules/tl20/src/cpp/tl/support/display/DisplayItem_List.cpp",
    "modules/tl20/src/cpp/tl/support/display/DisplayContext.cpp",
    "modules/tl20/src/cpp/tl/support/display/DisplayItem.cpp",
    "modules/tl20/src/cpp/tl/support/Displayer.cpp",

    "modules/tl20/src/cpp/tl/support/string/read_arg_name.cpp",

    "src/cpp/PowerDiagram/VtkOutput.cpp",

    "tests/cpp/SdotSolver.cpp",
]

args = dict( ARGLIST )
test = args[ 'test' ]

if 'test' in args:
    env = make_env()
    env.Program( f'build/{ test }.exe', [ f'tests/cpp/{ test }.cpp' ] + libs )
