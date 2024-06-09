#pragma once

#include <assert.h>

// #define STATIC_ASSERT_WITH_RETURN_IN_IF_CONSTEXPR( RET, COND, MSG ) do { static_assert( COND, MSG ); return RET; } while( false )
// #define STATIC_ASSERT_IN_IF_CONSTEXPR( COND, MSG ) static_assert( COND, MSG )

#define STATIC_ASSERT_WITH_RETURN_IN_IF_CONSTEXPR( RET, COND, MSG ) do { assert( COND ); return RET; } while( false )
#define STATIC_ASSERT_IN_IF_CONSTEXPR( COND, MSG ) assert( COND )
