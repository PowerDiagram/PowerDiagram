#pragma once

#include <tl/support/Displayer.h>

enum class CutType {
    Dirac,
    Boundary,
    Infinity,

};

void display( Displayer &ds, CutType t ) {
    switch ( t ) {
    case CutType::Dirac   : ds << "Dirac"   ; return;
    case CutType::Boundary: ds << "Boundary"; return;
    case CutType::Infinity: ds << "Infinity"; return;
    }
}
