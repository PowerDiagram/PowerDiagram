#pragma once

#include <tl/support/Displayer.h>

namespace sdot {
enum class CutType {
    Dirac,
    Boundary,
    Infinity,

};
} // namespace sdot 

inline void display( Displayer &ds, sdot::CutType t ) {
    switch ( t ) {
    case sdot::CutType::Dirac   : ds << "Dirac"   ; return;
    case sdot::CutType::Boundary: ds << "Boundary"; return;
    case sdot::CutType::Infinity: ds << "Infinity"; return;
    }
}
