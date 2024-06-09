#pragma once

#include "common_macros.h"

BEG_METIL_NAMESPACE

// // args for ctor selection
// #define DECL_VOID_STRUCT_TTT( NAME, ... ) template<class T> struct NAME { static auto base_type_name() { return #NAME; } static void for_each_template_arg( auto &&f ) { f( S<T>() ); } }
#define DECL_VOID_STRUCT_STD( NAME ) struct NAME { static auto type_name() { return #NAME; } }

// DECL_VOID_STRUCT_STD( FromSizeAndUninitializedItemValues ); ///<
// DECL_VOID_STRUCT_STD( FromSizeAndExternalItemValues  ); ///< will make a ref
// DECL_VOID_STRUCT_STD( FromSizeAndItemIndexFunction   ); ///< fill with a init function
// DECL_VOID_STRUCT_STD( FromSizeAndInitItemFunction    ); ///< fill with a init function
// DECL_VOID_STRUCT_STD( FromSizeAndItemValues          ); ///< will make a copy
// DECL_VOID_STRUCT_STD( FromSizeAndItemValue           ); ///< fill-like
// DECL_VOID_STRUCT_TTT( FromTypedItemValues            ); ///< will make a copy, with a forced type
// DECL_VOID_STRUCT_STD( FromMovableIterator            );
// DECL_VOID_STRUCT_STD( FromIterator                   );
// DECL_VOID_STRUCT_STD( FromExternalValue              ); ///< will make a ref
// DECL_VOID_STRUCT_STD( FromInitFunction               );
// DECL_VOID_STRUCT_STD( Uninitialized                  );

DECL_VOID_STRUCT_STD( FromSizeAndInitFunctionOnIndex );
DECL_VOID_STRUCT_STD( FromTypeAndCtorArguments       );
DECL_VOID_STRUCT_STD( FromOperationOnItemsOf         );
DECL_VOID_STRUCT_STD( FromPointerOnBorrowed          ); ///<
DECL_VOID_STRUCT_STD( FromShapeAndItemValue          );
DECL_VOID_STRUCT_STD( FromShapeAndIterator           );
DECL_VOID_STRUCT_STD( FromSizeAndItemValue           );
DECL_VOID_STRUCT_STD( FromSizeAndIterator            );
DECL_VOID_STRUCT_STD( FromReservationSize            ); ///<
DECL_VOID_STRUCT_STD( FromShapeAndValues             );
DECL_VOID_STRUCT_STD( FromPointerOnOwned             ); ///<
DECL_VOID_STRUCT_STD( FromConcatenation              );
DECL_VOID_STRUCT_STD( FromTupleValues                ); ///<
DECL_VOID_STRUCT_STD( FromAttributes                 );
DECL_VOID_STRUCT_STD( FromItemValues                 ); ///< will make copies
DECL_VOID_STRUCT_STD( FromItemValue                  );
DECL_VOID_STRUCT_STD( FromIterator                   ); ///<
DECL_VOID_STRUCT_STD( FromValue                      );
DECL_VOID_STRUCT_STD( FromSize                       );

DECL_VOID_STRUCT_STD( NoConstruction                 );

#undef DECL_VOID_STRUCT_STD

END_METIL_NAMESPACE
