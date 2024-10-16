#pragma once

#include <tl/support/Displayer.h>

namespace power_diagram {
enum class CutType {
    Dirac,
    Boundary,
    Infinity,

};
} // namespace power_diagram 

inline void display( Displayer &ds, power_diagram::CutType t ) {
    switch ( t ) {
    case power_diagram::CutType::Dirac   : ds << "Dirac"   ; return;
    case power_diagram::CutType::Boundary: ds << "Boundary"; return;
    case power_diagram::CutType::Infinity: ds << "Infinity"; return;
    }
}
