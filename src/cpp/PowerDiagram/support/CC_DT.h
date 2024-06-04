#pragma once

#ifndef PD_DIM
#define PD_DIM 2
#endif // PD_DIM

#ifndef PD_TYPE
#define PD_TYPE double
#endif // PD_TYPE


#define CONCAT_5( A, B, C, D, E ) A ## B ## C ## D ## E
#define CC_DT_( NAME, D, T ) CONCAT_5( NAME, _, D, _, T )

#define CC_DT( NAME ) CC_DT_( NAME, PD_DIM, PD_TYPE )
