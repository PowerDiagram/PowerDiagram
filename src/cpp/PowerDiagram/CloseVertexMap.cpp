#pragma once

#include "CloseVertexMap.h"

#define DTP template<class Point,class Value>
#define UTP CloseVertexMap<Point,Value>

DTP Value &UTP::operator[]( const Point &point ) {
    return map[ point ];
}


#undef DTP
#undef UTP

