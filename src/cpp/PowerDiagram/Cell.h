#pragma once

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
    void          init              ( const Point *orig_point, const Scalar *orig_weight, PI orig_index );
    void          cut               ( const Point &dir, Scalar off, PI cut_index );

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
