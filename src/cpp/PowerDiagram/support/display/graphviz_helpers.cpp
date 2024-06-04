#include "graphviz_helpers.h"
#include <iostream>

BEG_METIL_NAMESPACE

int exec_dot( Str f ) {
    // call graphviz
    Str cmd = "dot -Tpdf " + f + " > " + f + ".pdf";
    if ( int res = system( cmd.c_str() ) )
        return res;

    // call viewer
    cmd = "l=`qdbus --session | grep okular`;\nu=`for i in $l; do gdbus call --session --dest $i --object-path /okular --method org.kde.okular.currentDocument; done`;\n( echo $u | grep " + f + ".pdf ) || okular " + f + ".pdf &";
    if ( int res = system( cmd.c_str() ) )
        std::cerr << "error calling pdf display (" << res << ")" << std::endl;
    return 0;
}

END_METIL_NAMESPACE

