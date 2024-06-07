#pragma once

#include "support/compare.h"
#include <map>

/**
 * @brief 
 * 
 */
template<class Point,class Value>
class CloseVertexMap {
public:
    using  Map       = std::map<Point,Value,Less>;

    Value &operator[]( const Point &point );

    Map    map;      ///<
};
