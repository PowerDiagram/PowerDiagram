#pragma once

#include "support/display/DisplayItemFactory.h"
#include "Vertex.h"
#include "Edge.h"
#include "Cut.h"

#define Cell CC_DT( Cell )

/**
 * @brief 
 * 
 */
class Cell { STD_METIL_TYPE_INFO( Cell, "", vertices, edges, cuts )
public:
    /**/          Cell              ();
    /**/         ~Cell              ();

    DisplayItem*  display           ( DisplayItemFactory &df ) const;

    const Scalar* orig_weight;      ///<
    const Point*  orig_point;       ///<
    PI            orig_index;       ///<

    Vec<Vertex>   vertices;         ///<
    Vec<Edge>     edges;            ///<
    Vec<Cut>      cuts;             ///<

private:
    // FaceToInt  waiting_vertices; ///<
    Vec<int>      vertex_corr;      ///<
    Vec<int>      edge_corr;        ///<
    Vec<Scalar>   sps;              ///<

    mutable PI    curr_op_id = 0;   ///<
};
