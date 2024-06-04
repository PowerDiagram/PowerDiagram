#pragma once

#include <iostream> // IWYU pragma: export
#include <assert.h> // IWYU pragma: export

#define TODO \
    do { std::cerr << __FILE__ << ":" << __LINE__ << ": TODO; "; assert( 0 ); } while ( 0 )
